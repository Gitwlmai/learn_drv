#include "led_opr.h"
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

static volatile unsigned int *p_iomuxc_ctl_pad;
static volatile unsigned int *p_gpio_dr;
static volatile unsigned int *p_gpio_dir;

void led_init(int which)
{
    if(which == 0)
    {
        //ioremap 
        p_iomuxc_ctl_pad = ioremap(0x2290000 + 0x14, 4);
        p_gpio_dr = ioremap(0x20AC000, 4);
        p_gpio_dir = ioremap(0x20AC004, 4);

        //init pad as gpio
        *p_iomuxc_ctl_pad &= ~0x0F;
        *p_iomuxc_ctl_pad |= 0x05;

        //init gpio dir is output
        *p_gpio_dir |= (1<<3);
    }

}

void led_deinit(int which)
{
    if(which == 0)
    {
        iounmap(p_iomuxc_ctl_pad);
        iounmap(p_gpio_dr);
        iounmap(p_gpio_dir);
    }
}

void led_ctrl(int which, int status)
{
    if(which == 0)
    {
        if(status == 1)
        {
            printk("turn on the led\n");
            *p_gpio_dr &= ~(1<<3);
        }
        else
        {
            printk("turn off the led\n");
            *p_gpio_dr |= (1<<3);
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

led_opr_t *get_led_opr(void)
{
    return &g_led_opr;
}