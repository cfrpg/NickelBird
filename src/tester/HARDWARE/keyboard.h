#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "sys.h"
#include "delay.h"

#define GPQ PEin(4)
#define GPCLK PEout(3)
#define GPSHIFT PEout(2)
#define WHEEL PAin(12)
#define WHEELPUSH PCin(7) 

#define KEY_A 0x01
#define KEY_B 0x02
#define KEY_C 0x04
#define KEY_D 0x08
#define KEY_UP 0x10
#define KEY_LEFT 0x20
#define KEY_RIGHT 0x40
#define KEY_DOWN 0x80

void KeyInit(void);
u8 GPGetData(void);
s32 WheelGetValue(void);
void WheelUpdate(void);
u8 KeyGetState(void);

#endif
