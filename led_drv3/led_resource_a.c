#include "led_resource.h"

led_resource_t g_resource_a = 
{
    .pin = GROUP_PIN(5, 3),
};


led_resource_t *get_led_resource(void)
{
    return &g_resource_a;
}