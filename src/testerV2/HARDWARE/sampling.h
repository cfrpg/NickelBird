#ifndef __SAMPLING_H
#define __SAMPLING_H
#include "sys.h"

#define EXT_TRIG_CLK	PEin(5)
#define EXT_TRIG_EN		PEin(6)
#define EXT_CLK_INV		PCout(0)
#define EXT_EN_INV		PCout(1)
#define SPL_TRIG_SEL	PDout(2)

#define INT_TRIG		PCout(6)

void SamplingInit(void);

void SamplingSlowUpdate(u16 time);
void SamplingFastUpdate(u16 time);
void SamplingIntUpdate(void);

#endif
