#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/module.h>

struct projext_led_data {
	struct led_classdev cdev;
	unsigned gpio;
	struct work_struct work;
	u8 new_level;
	u8 can_sleep;
	u8 active_low;
	u8 blinking;
	int (*platform_projext_blink_set)(unsigned gpio, int state,
			unsigned long *delay_on, unsigned long *delay_off);
};

static void projext_led_work(struct work_struct *work)
{
	struct projext_led_data	*led_dat =
		container_of(work, struct projext_led_data, work);
    printk(KERN_INFO "projext_led_work\n");

	if (led_dat->blinking) {
		led_dat->platform_projext_blink_set(led_dat->gpio,
						 led_dat->new_level,
						 NULL, NULL);
		led_dat->blinking = 0;
	} else
		gpio_set_value_cansleep(led_dat->gpio, led_dat->new_level);
}

/* extern unsigned long is_poweroff_halt_status; */

static void projext_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	struct projext_led_data *led_dat =
		container_of(led_cdev, struct projext_led_data, cdev);
	int level;
    printk(KERN_INFO "projext_led_set\n");

	if (value == LED_OFF)
		level = 0;
	else
		level = 1;

	if (led_dat->active_low)
		level = !level;

	/* Setting GPIOs with I2C/etc requires a task context, and we don't
	 * seem to have a reliable way to know if we're already in one; so
	 * let's just assume the worst.
	 */
	if (led_dat->can_sleep) {
		led_dat->new_level = level;
		schedule_work(&led_dat->work);
	} else {
		if (led_dat->blinking) {
			led_dat->platform_projext_blink_set(led_dat->gpio, level,
							 NULL, NULL);
			led_dat->blinking = 0;
		} else {
			//if (is_poweroff_halt_status == 0)
			//	gpio_set_value(led_dat->gpio, level);
		}
	}
}

static int projext_blink_set(struct led_classdev *led_cdev,
	unsigned long *delay_on, unsigned long *delay_off)
{
	struct projext_led_data *led_dat =
		container_of(led_cdev, struct projext_led_data, cdev);
    printk(KERN_INFO "projext_blink_set\n");

	led_dat->blinking = 1;
	return led_dat->platform_projext_blink_set(led_dat->gpio, GPIO_LED_BLINK,
						delay_on, delay_off);
}

static int __devinit create_projext_led(struct gpio_led *template,
	struct projext_led_data *led_dat, struct device *parent,
	int (*blink_set)(unsigned, int, unsigned long *, unsigned long *))
{
	int ret, state;
    printk(KERN_INFO "create_projext_led\n");

	led_dat->gpio = -1;

	/*
	 * initialize gpio number for multiple b/d supporting
	 */
	if (template->gpio == (unsigned)-1)
	{
		if (strcmp(template->name, "af") == 0)
			template->gpio = GPIO_AF_LED;
		else if (strcmp(template->name, "card") == 0)
			template->gpio = GPIO_CARD_LED;
	}
	
	/* skip leds that aren't available */
	if (!gpio_is_valid(template->gpio)) {
		printk(KERN_INFO "Skipping unavailable LED gpio %d (%s)\n",
				template->gpio, template->name);
		return 0;
	}

	ret = gpio_request(template->gpio, template->name);
	if (ret < 0)
		return ret;

	led_dat->cdev.name = template->name;
	led_dat->cdev.default_trigger = template->default_trigger;
	led_dat->gpio = template->gpio;
	led_dat->can_sleep = gpio_cansleep(template->gpio);
	led_dat->active_low = template->active_low;
	led_dat->blinking = 0;
	if (blink_set) {
		led_dat->platform_projext_blink_set = blink_set;
		led_dat->cdev.blink_set = projext_blink_set;
	}
	led_dat->cdev.brightness_set = projext_led_set;
	if (template->default_state == LEDS_GPIO_DEFSTATE_KEEP)
		state = !!gpio_get_value_cansleep(led_dat->gpio) ^ led_dat->active_low;
	else
		state = (template->default_state == LEDS_GPIO_DEFSTATE_ON);
	led_dat->cdev.brightness = state ? LED_FULL : LED_OFF;
	if (!template->retain_state_suspended)
		led_dat->cdev.flags |= LED_CORE_SUSPENDRESUME;

	ret = gpio_direction_output(led_dat->gpio, led_dat->active_low ^ state);
	if (ret < 0)
		goto err;
		
	INIT_WORK(&led_dat->work, projext_led_work);

	ret = led_classdev_register(parent, &led_dat->cdev);
	if (ret < 0)
		goto err;

	return 0;
err:
	gpio_free(led_dat->gpio);
	return ret;
}

static void delete_projext_led(struct projext_led_data *led)
{
    printk(KERN_INFO "delete_projext_led\n");
	if (!gpio_is_valid(led->gpio))
		return;
	led_classdev_unregister(&led->cdev);
	cancel_work_sync(&led->work);
	gpio_free(led->gpio);
}

struct projext_leds_priv {
	int num_leds;
	struct projext_led_data leds[];
};

static inline int sizeof_projext_leds_priv(int num_leds)
{
	return sizeof(struct projext_leds_priv) +
		(sizeof(struct projext_led_data) * num_leds);
}

/* Code to create from OpenFirmware platform devices */
#ifdef CONFIG_OF_GPIO
static struct projext_leds_priv * __devinit projext_leds_create_of(struct platform_device *pdev)
{
    printk(KERN_INFO "projext_leds_create_of\n");
	struct device_node *np = pdev->dev.of_node, *child;
	struct projext_leds_priv *priv;
	int count = 0, ret;

	/* count LEDs in this device, so we know how much to allocate */
	for_each_child_of_node(np, child)
		count++;
	if (!count)
		return NULL;

	priv = kzalloc(sizeof_projext_leds_priv(count), GFP_KERNEL);
	if (!priv)
		return NULL;

	for_each_child_of_node(np, child) {
		struct gpio_led led = {};
		enum of_gpio_flags flags;
		const char *state;

		led.gpio = of_get_gpio_flags(child, 0, &flags);
		led.active_low = flags & OF_GPIO_ACTIVE_LOW;
		led.name = of_get_property(child, "label", NULL) ? : child->name;
		led.default_trigger =
			of_get_property(child, "linux,default-trigger", NULL);
		state = of_get_property(child, "default-state", NULL);
		if (state) {
			if (!strcmp(state, "keep"))
				led.default_state = LEDS_GPIO_DEFSTATE_KEEP;
			else if (!strcmp(state, "on"))
				led.default_state = LEDS_GPIO_DEFSTATE_ON;
			else
				led.default_state = LEDS_GPIO_DEFSTATE_OFF;
		}

		ret = create_projext_led(&led, &priv->leds[priv->num_leds++],
				      &pdev->dev, NULL);
		if (ret < 0) {
			of_node_put(child);
			goto err;
		}
	}

	return priv;

err:
	for (count = priv->num_leds - 2; count >= 0; count--)
		delete_projext_led(&priv->leds[count]);
	kfree(priv);
	return NULL;
}

static const struct of_device_id of_projext_match[] = {
	{ .compatible = "gpio-leds", },
	{},
};
#else /* CONFIG_OF_GPIO */
static struct projext_leds_priv * __devinit projext_leds_create_of(struct platform_device *pdev)
{
	return NULL;
}
#define of_projext_match NULL
#endif /* CONFIG_OF_GPIO */


static int __devinit projext_probe(struct platform_device *pdev)
{
	struct gpio_led_platform_data *pdata = pdev->dev.platform_data;
	struct projext_leds_priv *priv;
	int i, ret = 0;
    printk(KERN_INFO "projext_probe\n");

	if (pdata && pdata->num_leds) {
		priv = kzalloc(sizeof_projext_leds_priv(pdata->num_leds),
				GFP_KERNEL);
		if (!priv)
			return -ENOMEM;

		priv->num_leds = pdata->num_leds;
		for (i = 0; i < priv->num_leds; i++) {
			ret = create_projext_led(&pdata->leds[i],
					      &priv->leds[i],
					      &pdev->dev, pdata->gpio_blink_set);
			if (ret < 0) {
				/* On failure: unwind the led creations */
				for (i = i - 1; i >= 0; i--)
					delete_projext_led(&priv->leds[i]);
				kfree(priv);
				return ret;
			}
		}
	} else {
		priv = projext_leds_create_of(pdev);
		if (!priv)
			return -ENODEV;
	}

	platform_set_drvdata(pdev, priv);

	return 0;
}

static int __devexit projext_remove(struct platform_device *pdev)
{
	struct projext_leds_priv *priv = dev_get_drvdata(&pdev->dev);
	int i;
    printk(KERN_INFO "projext_remove\n");

	if(priv !=NULL){		
		for (i = 0; i < priv->num_leds; i++)
			delete_projext_led(&priv->leds[i]);

		dev_set_drvdata(&pdev->dev, NULL);
		kfree(priv);
	}
	return 0;
}

static struct platform_driver projext_driver = {
	.probe		= projext_probe,
	.remove		= __devexit_p(projext_remove),
	.driver		= {
		.name	= "projext",
		.owner	= THIS_MODULE,
		.of_match_table = of_projext_match,
	},
};

MODULE_ALIAS("platform:projext");
static int __init projext_init(void)
{
    printk(KERN_INFO "projext_init\n");
	return platform_driver_register(&projext_driver);
}

static void __exit projext_exit(void)
{
    printk(KERN_INFO "projext_exit\n");
	platform_driver_unregister(&projext_driver);
}

#ifndef CONFIG_SCORE_FAST_RESUME
module_init(projext_init);
#else
fast_dev_initcall(projext_init);
#endif
module_exit(projext_exit);
MODULE_AUTHOR("matt daughtrey");
MODULE_DESCRIPTION("Projector extensions");
MODULE_LICENSE("GPL");

