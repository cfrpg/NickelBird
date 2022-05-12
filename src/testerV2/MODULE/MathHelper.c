#include "MathHelper.h"

float Clampf(float v,float min,float max)
{
	if(v<min)
		return min;
	if(v>max)
		return max;
	return v;
}

s32 Clampi(s32 v,s32 min,s32 max)
{
	if(v<min)
		return min;
	if(v>max)
		return max;
	return v;
}

float AngleSub(float a,float b)
{
	if(a>b)
		return a-b;
	return a+TwoPI-b;
}
