#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/io.h>

static int major = 0;
static struct class *led_class;

//list GPIO5_3 register

//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3

static volatile unsigned int *p_iomuxc_ctl_pad;
static volatile unsigned int *p_gpio_dr;
static volatile unsigned int *p_gpio_dir;
//init pad as gpio
//init gpio dir is output
//set gpio val 1 or 0

int led_open(struct inode *inode, struct file *file)
{
    //init pad as gpio
    *p_iomuxc_ctl_pad &= ~0x0F;
    *p_iomuxc_ctl_pad |= 0x05;

    //init gpio dir is output
    *p_gpio_dir |= (1<<3);

    return 0;
}

ssize_t led_write(struct file *file, const char __user *user, size_t size, loff_t *loff)
{
    char val = 0;
    int ret = 0;
    ret = copy_from_user(&val, user, 1);
    if(val == 1)
    {
        printk("turn on the led\n");
        *p_gpio_dr &= ~(1<<3);
    }
    else
    {
        printk("turn of the led\n");
        *p_gpio_dr |= (1<<3);
    }

    return ret;
}


static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.open		= led_open,
	.write      = led_write,
};


static int __init led_init(void)
{
    int err = 0;
    printk("insmod %s %s %d \n", __FILE__, __func__, __LINE__);
    major = register_chrdev(0, "led_chrdev", &led_fops);
    if (major < 0) 
    {
		pr_err("%s unable to get major \n", __func__);
		return major;
	}
    led_class = class_create(THIS_MODULE, "led_class");
    if(IS_ERR(led_class))
    {
        unregister_chrdev(major, "led_chrdev");
        err = PTR_ERR(led_class);
        return err;
    }

    device_create(led_class, NULL, MKDEV(major, 0), NULL, "%s", "led_device");


    p_iomuxc_ctl_pad = ioremap(0x2290000 + 0x14, 4);
    p_gpio_dr = ioremap(0x20AC000, 4);
    p_gpio_dir = ioremap(0x20AC004, 4);

    return err;
}

static void __exit led_exit(void)
{
    device_destroy(led_class, MKDEV(major, 0));
    class_destroy(led_class);
    unregister_chrdev(major, "led_chrdev");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");