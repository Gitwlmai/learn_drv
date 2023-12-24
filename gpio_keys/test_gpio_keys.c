#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>

struct gpio_keys_res 
{
    int gpio;
    int irq;
    struct gpio_desc *desc;
    enum of_gpio_flags flags;
};

struct gpio_keys_res *g_keys_res = NULL;


irqreturn_t gpio_keys_irq_handler(int irq, void *dev_id)
{
    struct gpio_keys_res *g_keys_res = (struct gpio_keys_res *)dev_id;
    printk("irq %d, value %d\n", irq, gpiod_get_value(g_keys_res->desc));

    return 0;
}

static int test_gpio_keys_probe(struct platform_device *device)
{
    struct device_node *node = device->dev.of_node;
    int count = 0;
    int i = 0;
    int ret = 0;
    unsigned flags = GPIOF_IN;
    //get gpio info from dts
    count = of_gpio_count(node);
    g_keys_res = kzalloc(count * sizeof(struct gpio_keys_res), GFP_KERNEL);
    for(i = 0; i < count; i++)
    {
        g_keys_res[i].gpio = of_get_gpio_flags(node, i, &g_keys_res[i].flags);
        if(g_keys_res[i].flags & OF_GPIO_ACTIVE_LOW)
        {
            flags |= GPIOF_ACTIVE_LOW;
		    devm_gpio_request_one(&device->dev, g_keys_res[i].gpio, flags, NULL);
        }

        g_keys_res[i].irq = gpio_to_irq(g_keys_res[i].gpio);
        g_keys_res[i].desc = gpio_to_desc(g_keys_res[i].gpio);

        ret = request_irq(g_keys_res[i].irq, gpio_keys_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "mwl,gpio_keys_int", &g_keys_res[i]);
    }

    
    return 0;
}


static int test_gpio_keys_remove(struct platform_device *device)
{
    int count = of_gpio_count(device->dev.of_node);
    int i = 0;
	for (i = 0; i < count; i++)
	{
		free_irq(g_keys_res[i].irq, &g_keys_res[i]);
	}
    
    kfree(g_keys_res);
    g_keys_res = NULL;

    return 0;
}

static const struct of_device_id gpio_keys_of_match[] = {
	{ .compatible = "test,gpio-keys", },
	{ },
};

static struct platform_driver test_gpio_keys_driver = {
	.probe		= test_gpio_keys_probe,
	.remove		= test_gpio_keys_remove,
	.driver		= {
		.name	= "test,gpio-keys",
		.of_match_table = of_match_ptr(gpio_keys_of_match),
	}
};

static int __init test_gpio_key_init(void)
{
    platform_driver_register(&test_gpio_keys_driver);
    return 0;
}

static void __exit test_gpio_key_exit(void)
{
    platform_driver_unregister(&test_gpio_keys_driver);
}

module_init(test_gpio_key_init);
module_exit(test_gpio_key_exit);
MODULE_LICENSE("GPL");