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
#include "led_opr.h"

static int major = 0;
static struct class *led_class;
static led_opr_t *g_led_opr;

int led_open(struct inode *inode, struct file *file)
{
    int i = iminor(inode);
    g_led_opr->init(i);

    return 0;
}


ssize_t led_write(struct file *file, const char __user *user, size_t size, loff_t *loff)
{
    char val = 0;
    int ret = 0;
    int i = iminor(file_inode(file));
    ret = copy_from_user(&val, user, 1);
    g_led_opr->ctrl(i, val);
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

    return err;
}

void led_register_opr(led_opr_t *opr)
{
    g_led_opr = opr;
}

EXPORT_SYMBOL(led_register_opr);

void led_device_create(int i)
{
    printk("call device_create \n");
    device_create(led_class, NULL, MKDEV(major, i), NULL, "led_device%d", i); 
}

void led_device_destroy(int i)
{
    device_destroy(led_class, MKDEV(major, i));
}

EXPORT_SYMBOL(led_device_create);
EXPORT_SYMBOL(led_device_destroy);


static void __exit led_exit(void)
{    
    class_destroy(led_class);
    unregister_chrdev(major, "led_chrdev");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");