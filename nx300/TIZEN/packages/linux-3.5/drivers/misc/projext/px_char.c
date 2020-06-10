#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <linux/fs.h>       /* Header for linux FS support */
#include <linux/device.h>
#include <asm/uaccess.h>    /* For copy to user function */
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/timer.h>
//#include "fio.c"

#define DRIVER_AUTHOR "Matt Daughtrey matt@daughtrey.com"
#define DRIVER_DESC   "Projector Extensions"
#define DEVICE_NAME "projext"
#define CLASS_NAME "projext"

static int majorNumber;
static struct class *pecharClass = NULL;
static struct device * pecharDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static irqreturn_t projext_isr(int irq, void *handle);
static int monGpio = -1;
static struct timer_list sdTimer;
static int sdTimeout = -1;

static struct file_operations fops = 
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release
};

static int __init init_projext(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "PXC: Could not register a major number\n");
        return majorNumber;
    }

    pecharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(pecharClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PXC: Failed to register device class\n");
        return PTR_ERR(pecharClass);
    }

    pecharDevice = device_create(pecharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(pecharDevice))
    {
        class_destroy(pecharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PXC: Failed to create the device\n");
        return PTR_ERR(pecharDevice);
    }

    request_irq(199, projext_isr, IRQF_DISABLED, "power_key", NULL);
    request_irq(200, projext_isr, IRQF_DISABLED, "power_key", NULL);
    request_irq(290, projext_isr, IRQF_DISABLED, "power_key", NULL);
    return 0;
}

static void __exit cleanup_projext(void)
{
    free_irq(199, NULL);
    free_irq(200, NULL);
    free_irq(290, NULL);
    del_timer(&sdTimer);
    device_destroy(pecharClass, MKDEV(majorNumber, 0));
    class_unregister(pecharClass);
    class_destroy(pecharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
}

module_init(init_projext);
module_exit(cleanup_projext);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);   /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);    /* What does this module do */
MODULE_VERSION("0.1");

// ---------------------------------------------------
// Device operation handlers 
// ---------------------------------------------------
static int dev_open(struct inode * inodep, struct file * filep)
{
    return 0;
}

static int dev_release(struct inode * inodep, struct file * filep)
{
    return 0;
}

static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t * offset)
{
    char local[8];
    if (-1 == sdTimeout)
    {
        printk(KERN_INFO "PXC: sdTimeout not set\n");
        return 0;
    }
    sprintf(local, "%u", sdTimeout);
    copy_to_user(buffer, local, 1);
    //monGpio = -1;
    //sdTimeout = -1;
    printk(KERN_INFO "PXC: Returning %s\n", local);
    return 1;
}

static irqreturn_t projext_isr(int irq, void *handle)
{
    printk(KERN_INFO "PXC: projext_isr IRQ %d\n", irq);
    if (199 == irq) { gpio_set_value(193, 0); }
    if (200 == irq) { gpio_set_value(193, 1); }
    // if LED is on, reset timer
    if (290 == irq)
    {
        printk(KERN_INFO "PXC: CARD LED changed\n"); //  %u\n", gpio_get_value(194));
        printk(KERN_INFO "PXC: sdTimeout %d\n", sdTimeout);
        if (sdTimeout != -1)
        {
            printk(KERN_INFO "PXC: Resetting timer\n");
            mod_timer(&sdTimer, jiffies + msecs_to_jiffies(1000));
        }
    }
    return IRQ_HANDLED;
}
static void timerCallback(unsigned long data)
{
    printk(KERN_INFO "PXC: TimerCallback\n");
    sdTimeout = 1;
}

static ssize_t dev_write(struct file * filep, const char * buffer, size_t len, loff_t * offset)
{
    int gpionum;
    int state;
    char operation[8];

    printk(KERN_INFO "PXC: dev_write\n");

    if (2 != sscanf(buffer, "%s %d", operation, &gpionum))
    {
        printk(KERN_INFO "PXC: sscanf fails\n");
        return len;
    }
    if (0 == strncmp("IRQ", operation, 3))
    {
        state = gpio_to_irq(gpionum);
        printk(KERN_INFO "PXC: %d maps to %d\n", gpionum, state);
    }
    else if (0 == strncmp("IO", operation, 2))
    {
        if (3 != sscanf(buffer, "%s %d %d", operation, &gpionum, &state))
        {
            printk(KERN_INFO "PXC: IO sscanf fails\n");
            return len;
        }
        printk(KERN_INFO "PXC: %s GPIO %d state %d\n", operation, gpionum, state);
        gpio_set_value(gpionum, state);
    }
    else if (0 == strncmp("MONT", operation, 4))
    {
        printk(KERN_INFO "PXC: Timed Monitor\n"); //  GPIO %d\n", gpionum);
        monGpio = gpionum;
        setup_timer(&sdTimer, timerCallback, 0);
        sdTimeout = 0;
    }
    //else if (0 == strncmp("MON", operation, 3))
    //{
    //    printk(KERN_INFO "PXC: Monitor GPIO %d\n", gpionum);
    //    //monGpio = gpionum;
    //}
    return len;
}

