#ifndef _LED_RESOURCE_H_
#define _LED_RESOURCE_H_

#define GROUP_PIN(group, pin) ((group << 16) | pin)
#define GROUP(gp) ((gp >> 16) & 0xFFFF)
#define PIN(gp) (gp & 0xFFFF)

#endif