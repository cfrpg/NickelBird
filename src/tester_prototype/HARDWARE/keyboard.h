#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "sys.h"
#include "delay.h"

#define GPQ PDin(9)
#define GPCLK PDout(10)
#define GPSHIFT PDout(11)
#define WHEEL PDin(12)

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

#endif
