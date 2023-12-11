#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include "led_resource.h"

static struct resource led_resources[] = {
	[0] = {		
		.start	= GROUP_PIN(5, 3),
		.flags	= IORESOURCE_IRQ,
	},
	[1] = {		
		.start	= GROUP_PIN(4, 6),
		.flags	= IORESOURCE_IRQ,
	},
    [2] = {		
		.start	= GROUP_PIN(7, 2),
		.flags	= IORESOURCE_IRQ,
	},
};

static void led_dev_release(struct device *dev)
{
}

static struct platform_device g_led_device = {
	.name	= "mwl,led",
	.resource = led_resources,
	.num_resources	= ARRAY_SIZE(led_resources),
    .dev = {
                .release = led_dev_release,
    },
};


static int __init led_dev_init(void)
{
    printk("call platform_device_register .. \n");
    platform_device_register(&g_led_device);

    return 0;
}

static void __exit led_dev_exit(void)
{
    platform_device_unregister(&g_led_device);
}

module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");