#ifndef __NICKELBIRD_H
#define __NICKELBIRD_H

#include "sys.h"
#include "sblink.h"
#include "sampling.h"

typedef struct
{
	//SamplingState spl;
	
	SensorDataPackage sensors;
	u16 pwm[4];
	float freq[2];
	void (*slowUpdate)(u16);
	void (*fastUpdate)(u16);
	void (*intUpdate)(void);
	void (*intReset)(void);
	u8 intFlag[2];
	u32 intTime[2];
	u32 EOtime;
	u32 EOinterval;
	u16 ledInterval;

} systemState;

extern systemState sys;

#endif
