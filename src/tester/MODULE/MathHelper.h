#ifndef __MATHHELPER_H
#define __MATHHELPER_H

#include "sys.h"

#define PI 3.1415926f
#define TwoPI 6.283185307f

float Clampf(float v,float min,float max);
s32 Clampi(s32 v,s32 min,s32 max);


float AngleSub(float a,float b);


#endif
