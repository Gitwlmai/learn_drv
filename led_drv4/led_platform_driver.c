#include "led_opr.h"
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include "led_resource.h"

static unsigned int g_led_pin[10];
static unsigned int g_led_cnt = 0;

void led_register_opr(led_opr_t *opr);
void led_device_destroy(int i);
void led_device_create(int i);

void led_init(int which)
{
    printk("led_init group %d, pin %d \n", GROUP(g_led_pin[which]), PIN(g_led_pin[which]));     
}

void led_deinit(int which)
{
    
}

void led_ctrl(int which, int status)
{
    printk("led_ctrl group %d, pin %d, status %d\n", GROUP(g_led_pin[which]), PIN(g_led_pin[which]), status);     
}

led_opr_t g_led_opr =
{
    .num = 2,
    .init = led_init,
    .deinit = led_deinit,
    .ctrl = led_ctrl,
};


static int led_driver_probe(struct platform_device *pdev)
{
    struct resource *res;
    int i = 0;
    printk("call led_driver_probe \n");
    while(1)
    {
        //get resource
        res = platform_get_resource(pdev, IORESOURCE_IRQ, i++);
        if(!res)
        {
            break;
        }
        printk("get %d led info \n", g_led_cnt);
        led_device_create(g_led_cnt);
        g_led_pin[g_led_cnt++] = res->start;
    }
    
    return 0;
}

static int led_driver_remove(struct platform_device *pdev)
{
    int i = 0;
    for(i = 0; i < g_led_cnt; i++)
    {
        led_device_destroy(i);
    }
    g_led_cnt = 0;
    printk("call led_driver_remove \n");

    return 0;
}

static struct platform_driver g_led_driver = {
	.driver = {
		.name = "mwl,led",
	},
	.probe = led_driver_probe,
	.remove = led_driver_remove,
};

static int __init led_drv_init(void)
{
    platform_driver_register(&g_led_driver);
    led_register_opr(&g_led_opr);
    return 0;
}

static void __exit led_drv_exit(void)
{
    platform_driver_unregister(&g_led_driver);
}

module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");