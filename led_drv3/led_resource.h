#ifndef _LED_RESOURCE_H_
#define _LED_RESOURCE_H_


//16~31 group
//0~15 pin
#define GROUP_PIN(group, pin) ((group << 16) | (pin))
#define GROUP(gp) ((gp >> 16) & 0xFFFF)
#define PIN(gp) (gp & 0xFFFF)


typedef struct led_resource
{
    int pin;
}led_resource_t;


led_resource_t *get_led_resource(void);


#endif