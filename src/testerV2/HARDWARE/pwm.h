#ifndef __PWM_H
#define __PWM_H
#include "sys.h"
#include "parameter.h"


void PWMInit(void);
void PWMSet(u32 ch0,u32 ch1,u32 ch2,u32 ch3);
void PWMDisarm(void);
void PWMArm(void);
u8 PWMIsArmed(void);

#endif
