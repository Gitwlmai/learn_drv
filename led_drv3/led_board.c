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
#include "led_resource.h"

static led_resource_t *p_led_res;

void led_init(int which)
{
    p_led_res = get_led_resource();
}

void led_deinit(int which)
{
    
}

void led_ctrl(int which, int status)
{
    printk("group : %d, pin %d \n", GROUP(p_led_res->pin), PIN(p_led_res->pin));
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