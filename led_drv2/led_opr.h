#ifndef _LED_OPR_H_
#define _LED_OPR_H_


typedef struct led_opr
{
    int num;
    void (*init)(int which);
    void (*deinit)(int which);
    void (*ctrl)(int which, int status);
}led_opr_t;

led_opr_t *get_led_opr(void);


#endif