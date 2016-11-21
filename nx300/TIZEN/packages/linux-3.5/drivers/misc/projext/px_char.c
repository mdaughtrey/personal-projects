#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <linux/fs.h>       /* Header for linux FS support */
#include <linux/device.h>
#include <asm/uaccess.h>    /* For copy to user function */
#include <linux/interrupt.h>
#include <linux/slab.h>

//#include <linux/platform_device.h>
#include <linux/gpio.h>
//#include <linux/workqueue.h>
//#include <linux/leds.h>

#define DRIVER_AUTHOR "Matt Daughtrey matt@daughtrey.com"
#define DRIVER_DESC   "Projector Extensions"
#define DEVICE_NAME "projext"
#define CLASS_NAME "projext"

static int majorNumber;
static int numberOpens = 0;
static struct class *pecharClass = NULL;
static struct device * pecharDevice = NULL;
static int storedValue;
static char * params = "";
module_param(params, charp, 0000);
MODULE_PARM_DESC(params, "IRQ,IO,State[:IRQ,IO,State[:...]]");

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static irqreturn_t projext_isr(int irq, void *handle);
//static int __devinit projext_led_probe(struct platform_device *pdev);
//static int __devexit projext_led_remove(struct platform_device *pdev);
static int buildIrqList(void);

typedef struct 
{ 
    struct list_head list;
    int trigger;
    int output;
    int outstate;
} list_elem;

static struct file_operations fops = 
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release
};

static LIST_HEAD(irq_list);
//static const struct of_device_id of_projext_leds_match[] = {
//	{ .compatible = "projext", },
//	{},
//};
//
//static struct platform_driver projext_led_driver = {
//	.probe		= projext_led_probe,
//	.remove		= __devexit_p(projext_led_remove),
//	.driver		= {
//		.name	= "projext",
//		.owner	= THIS_MODULE
//		.of_match_table = of_projext_leds_match,
//	},
//};

//static int __devinit projext_led_probe(struct platform_device *pdev)
//{
//    printk(KERN_INFO "projext_led_probe\n");
	//struct gpio_led_platform_data *pdata = pdev->dev.platform_data;
	//struct gpio_leds_priv *priv;
	//int i, ret = 0;

	//if (pdata && pdata->num_leds) {
	//	priv = kzalloc(sizeof_gpio_leds_priv(pdata->num_leds),
	//			GFP_KERNEL);
	//	if (!priv)
	//		return -ENOMEM;

	//	priv->num_leds = pdata->num_leds;
	//	for (i = 0; i < priv->num_leds; i++) {
	//		ret = create_gpio_led(&pdata->leds[i],
	//				      &priv->leds[i],
	//				      &pdev->dev, pdata->gpio_blink_set);
	//		if (ret < 0) {
	//			/* On failure: unwind the led creations */
	//			for (i = i - 1; i >= 0; i--)
	//				delete_gpio_led(&priv->leds[i]);
	//			kfree(priv);
	//			return ret;
	//		}
	//	}
	//} else {
	//	priv = gpio_leds_create_of(pdev);
	//	if (!priv)
	//		return -ENODEV;
	//}

	//platform_set_drvdata(pdev, priv);

	//return 0;
//}

//static int __devexit projext_led_remove(struct platform_device *pdev)
//{
//    printk(KERN_INFO "projext_led_remove\n");
	//struct gpio_leds_priv *priv = dev_get_drvdata(&pdev->dev);
	//int i;

	//if(priv !=NULL){		
	//	for (i = 0; i < priv->num_leds; i++)
	//		delete_gpio_led(&priv->leds[i]);

	//	dev_set_drvdata(&pdev->dev, NULL);
	//	kfree(priv);
	//}
//	return 0;
//}
//
static int __init init_projext(void)
{
    printk(KERN_INFO "projext init\n");
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "Could not register a major number");
        return majorNumber;
    }
    printk(KERN_INFO "Registered major number %u\n", majorNumber);

    pecharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(pecharClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(pecharClass);
    }

    pecharDevice = device_create(pecharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(pecharDevice))
    {
        class_destroy(pecharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(pecharDevice);
    }

    printk(KERN_INFO "Looks like we're all registered\n");
    printk(KERN_INFO "Params %s\n", params);
    buildIrqList();

    return 0;
}

static void __exit cleanup_projext(void)
{
    printk(KERN_INFO "projext cleanup\n");
    device_destroy(pecharClass, MKDEV(majorNumber, 0));
    class_unregister(pecharClass);
    class_destroy(pecharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "And I'm spent\n");
}

static int buildIrqList(void)
{
    int trigger;
    int output;
    int state;
    char * pptr = params;
    int numsets = 0;
    while (*pptr)
    {
        int numread = sscanf(pptr, "%u,%u,%u", &trigger, &output, &state);
        if (3 != numread)
        {
            printk(KERN_INFO "sscanf fails, %u parameters read\n", numread);
            return -1;
        }
        printk(KERN_INFO "Trigger %u Output %u State %u\n", trigger, output, state);
#if 0
        list_elem * le = kzalloc(sizeof(list_elem), GFP_KERNEL);
        if (0 == le)
        {
            printk(KERN_INFO "le kzalloc fails\n");
            return -1;
        }
        le->trigger = trigger;
        le->output = output;
        le->outstate = state;
		list_add_tail(le->list, &irq_list);
#endif // 0
       pptr = strchr(pptr, ':');
       printk(KERN_INFO "pptr is %u\n", (int)pptr);
       if (pptr)
       {
           pptr++;
       }
       printk(KERN_INFO "pptr is %s\n", pptr);
    }
    return numsets;
}

module_init(init_projext);
module_exit(cleanup_projext);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);   /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);    /* What does this module do */
MODULE_VERSION("0.1");

//MODULE_SUPPORTED_DEVICE("mattd");
//module_param(greeting, charp, S_IRUGO);
//MODULE_PARM_DESC(greeting, "Greeting text");
//
// ---------------------------------------------------
// Device operation handlers 
// ---------------------------------------------------
static int dev_open(struct inode * inodep, struct file * filep)
{
    numberOpens++;
    printk(KERN_INFO "open: %u open instances\n", numberOpens);
    return 0;
}

static int dev_release(struct inode * inodep, struct file * filep)
{
    if (numberOpens > 0)
    {
        numberOpens--;
        printk(KERN_INFO "release: %u open instances\n", numberOpens);
    }
    else
    {
        printk(KERN_ALERT "Somehow closed more times than opened\n");
    }
    if (0 == numberOpens)
    {
#if 0
        struct list_head * list = NULL;
        while(!list_empty(&irq_list)) 
        {
            list_elem * le = list_entry(list, list_elem, list);
            if (le)
            {
                free_irq(le->irqnum, (void *)le);
                printk(KERN_INFO "Released IRQ %u\n", le->irqnum);
                list_del(&le->list);
                kfree(le);
            }
        }
#endif // 0
    }
    return 0;
}

static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t * offset)
{
    char tmp[32];
    if (-1 == storedValue)
    {
        return 0;
    }
    sprintf(tmp, "%d", storedValue);

    //int error_count = 0;
    printk(KERN_INFO "dev read\n");
	copy_to_user((void *)buffer, (const void *)tmp, strlen(tmp));
    printk(KERN_INFO "Buffer [%s]", buffer);
    //gpio_set_value(GPIO_AF_LED, 0);
    //gpio_set_value(GPIO_SHUTTER_KEY2, 1);
    //return tmp;
    storedValue = -1;
    return strlen(tmp);
}

static irqreturn_t projext_isr(int irq, void *handle)
{
    //list_elem * param = (list_elem *)handle;
    //if (param->outpin)
    //{
    //    gpio_set_value(param->outpin, param->outstate);
    //}
    return IRQ_HANDLED;
}
#if 0
static void dump_irq_list(struct list_head * irqlist)
{
	list_elem * elem = NULL;
	struct list_head *list;
	list_for_each(list, irqlist)
	{
		elem = list_entry(list, list_elem, list);
        printk(KERN_INFO "irqnum %d outpin %d outstate %d\n",
                elem->irqnum, elem->outpin, elem->outstate);
	}
}
#endif // 0

static ssize_t dev_write(struct file * filep, const char * buffer, size_t len, loff_t * offset)
{
    char cmd;
    int pin;
    int value;
    int retval = 0;
    //int outpin = 0;
    //int outstate = 0;
    list_elem * le = NULL;

//    printk(KERN_INFO "irq_list %x prev %x next %x\n", 
//            (int)&irq_list, (int)irq_list.prev, (int)irq_list.next);
    if (1 != sscanf(buffer, "%c", &cmd))
    {
        printk(KERN_INFO "sscanf failed\n");
        return 0;
    }
    printk(KERN_INFO "Cmd %c\n", cmd);
    if ('r' == (cmd | 0x20) && 2 == sscanf(buffer, "%c %u", &cmd, &pin))
    {
        if ('r' == cmd)
        {
            retval = gpio_request(pin, "projext");
            if (0 != retval)
            {
                printk(KERN_INFO "gpio_request failed %d\n", retval);
                return retval;
            }
        }
        storedValue = gpio_get_value(pin);
        printk(KERN_INFO "Read value %u from pin %u\n", storedValue, pin);
        if ('r' == cmd)
        {
           gpio_free(pin);
        }
        return len;
    }
    if ('w' == (cmd|0x20) && 3 == sscanf(buffer, "%c %u %u", &cmd, &pin, &value))
    {
        if ('w' == cmd)
        {
            retval = gpio_request(pin, "projext");
            if (0 != retval)
            {
                printk(KERN_INFO "gpio_request failed %d\n", retval);
                return retval;
            }
        }
        gpio_set_value(pin, value);
        printk(KERN_INFO "Set pin %u to %u\n", pin, value);
        if ('w' == cmd)
        {
            gpio_free(pin);
        }
        return len;
    }
    if ('i' == cmd && 2 == sscanf(buffer, "%c %u", &cmd, &pin))
    {
		storedValue = gpio_to_irq(pin);
        printk(KERN_INFO "Pin %u maps to IRQ %u\n", pin, storedValue);
        return len;
    }
    //if ('I' == cmd && 4 == sscanf(buffer, "%c %u %u %u", 
    //            &cmd, &pin, &outpin, &outstate))
    //{
    //    printk(KERN_INFO "pin %d outpin %d outstate %d", pin, outpin, outstate);
	//	le = kzalloc(sizeof(list_elem), GFP_KERNEL);
    //    if (0 == le)
    //    {
    //        printk(KERN_INFO "le NULL\n");
    //        return -1;
    //    }
    //    printk(KERN_INFO "le %x\n", (int)le);
    //    le->irqnum = pin;
    //    le->outpin = outpin;
    //    le->outstate = outstate;

    //    le->list.next = &irq_list;
    //    le->list.prev = irq_list.prev;
    //    printk(KERN_INFO "1 %x\n", (int)irq_list.prev->next);
    //    printk(KERN_INFO "2 %x\n", (int)&le->list);
    //    
    //    //irq_list.prev->next = &le->list;
    //    irq_list.prev = &le->list;


	//	//retval = request_irq(pin, projext_isr, IRQF_DISABLED, "PXCHAR IRQ", (void*)le);
    //    //printk(KERN_INFO "request_irq %d\n", retval);
	//	//list_add_tail(&le->list, &irq_list);
    //    //dump_irq_list(&irq_list);
    //    return len;
    //}
    ////gpio_set_value(GPIO_AF_LED, 0);
    //return len;
    return -1;
}

