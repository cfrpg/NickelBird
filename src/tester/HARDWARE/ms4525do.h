#ifndef __MS4525DO_H
#define __MS4525DO_H
#include "sys.h"

#define MS4525DO_ADDR 0x51
#define MS4525DO_CS PGout(7)

void MS4525DOInit(void);
float MS4525DOReadAirspeed(float rho,float *pre);
float MS4525DOReadAverageAirspeed(void);
void MS4525DOCali(u8 n);
#endif
