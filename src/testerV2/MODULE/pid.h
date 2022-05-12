#ifndef __PID_H
#define __PID_H
#include "filter.h"

typedef struct
{
	Filter* lpf;
	float P;
	float I;
	float D;
	float err;
	float lasterr;
	float integral;
	float maxint;
	float derivate;
	float out;
	float maxout;
	float minout;
	float tau;
	u8 lpfEnabled;
} PIDData;

void PIDInit(PIDData *pid);
void PIDCalc(PIDData *pid,float err,float dt);

#endif
