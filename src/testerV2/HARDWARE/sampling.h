#ifndef __SAMPLING_H
#define __SAMPLING_H
#include "sys.h"

#define EXT_TRIG_CLK	PEin(5)
#define EXT_TRIG_EN		PEin(6)
#define EXT_CLK_INV		PCout(0)
#define EXT_EN_INV		PCout(1)
#define SPL_TRIG_SEL	PDout(2)

#define INT_TRIG		PCout(6)

#define INTERNAL 		0
#define EXTERNAL 		1
#define Failing			0
#define Raising			1
#define Active_High		0
#define Active_Low		1

typedef struct
{
	u16 InternalClockFrequency;
	u16 ExternalClockFrequency;
	u8 ClockSource;
	u8 ExternalClockPolarity;
	u8 ExternalEnabledPolarity;
	
	u8 adcBusy;
	
} SamplingState;

extern SamplingState splState;

void SamplingInit(void);
void SamplingSetClockSource(u8 t);
void SamplingSetInternalFrequency(u32 f);
void SamplingSetExternalClockPolarity(u8 p);
void SamplingSetExternalEnabledPolarity(u8 p);

void SamplingSlowUpdate(u16 time);
void SamplingFastUpdate(u16 time);
void SamplingIntUpdate(void);

#endif
