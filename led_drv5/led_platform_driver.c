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
#include <linux/of.h>
#include "led_resource.h"

static unsigned int g_led_pin[10];
static unsigned int g_led_cnt = 0;
static volatile unsigned int *p_iomuxc_ctl_pad[10] = {NULL};
static volatile unsigned int *p_gpio_dr[10] = {NULL};
static volatile unsigned int *p_gpio_dir[10] = {NULL};

void led_register_opr(led_opr_t *opr);
void led_device_destroy(int i);
void led_device_create(int i);

void led_init(int which)
{
    printk("led_init group %d, pin %d \n", GROUP(g_led_pin[which]), PIN(g_led_pin[which]));     
    if(which == 0)
    {
        *p_iomuxc_ctl_pad[which] &= ~0x0F;
        *p_iomuxc_ctl_pad[which] |= 0x05;

        //init gpio dir is output
        *p_gpio_dir[which] |= (1<<3);
    }
}

void led_deinit(int which)
{
    
}

void led_ctrl(int which, int status)
{
    printk("led_ctrl group %d, pin %d, status %d\n", GROUP(g_led_pin[which]), PIN(g_led_pin[which]), status);     
    if(which == 0)
    {
        if(status == 1)
        {
            printk("turn on the led\n");
            *p_gpio_dr[which] &= ~(1<<3);
        }
        else
        {
            printk("turn off the led\n");
            *p_gpio_dr[which] |= (1<<3);
        }

    }
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
    struct device_node *np;
    int led_pin = 0;
    printk("call led_driver_probe \n");

    //get node struct 
    np = pdev->dev.of_node;
    if(!np)
    {
        printk("device node null \n");
        return -1;
    }

    //get node property
    int err = of_property_read_u32(np, "pin", &led_pin);
    if(err != 0)
    {
        printk("of_property_read_u32 error \n");
        return -1;
    }

    
    g_led_pin[g_led_cnt] = led_pin;

    if(g_led_cnt == 0 && p_iomuxc_ctl_pad[g_led_cnt] == NULL)
    {
        p_iomuxc_ctl_pad[g_led_cnt] = ioremap(0x2290000 + 0x14, 4);
        p_gpio_dr[g_led_cnt] = ioremap(0x20AC000, 4);
        p_gpio_dir[g_led_cnt] = ioremap(0x20AC004, 4);
    }

    led_device_create(g_led_cnt);
    g_led_cnt++;
    
    return 0;
}

static int led_driver_remove(struct platform_device *pdev)
{
    struct device_node *np;
    int led_pin = 0;
    int i = 0;
    int tmp_led_cnt = g_led_cnt;
    //get node struct 
    np = pdev->dev.of_node;
    if(!np)
    {
        printk("device node null \n");
        return -1;
    }

    //get node property
    int err = of_property_read_u32(np, "pin", &led_pin);
    for(i = 0; i < tmp_led_cnt; i++)
    {
        if(led_pin == g_led_pin[i])
        {
            led_device_destroy(i);
            g_led_cnt--;
            break;
        }
    }

    if(led_pin == GROUP_PIN(5,3))
    {
        iounmap(p_iomuxc_ctl_pad[0]);
        iounmap(p_gpio_dr[0]);
        iounmap(p_gpio_dir[0]);
    }
 
    printk("call led_driver_remove \n");

    return 0;
}

static const struct of_device_id led_drv_match_table[] = {
	{
		.compatible = "mwl,leddrv",
	},
	{},
};


static struct platform_driver g_led_driver = {
	.driver = {
		.name = "mwl,led",
        .of_match_table = led_drv_match_table,
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