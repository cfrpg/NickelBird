#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

void ADCInit(void);
void ADCReadVol(short res[]);
void ADCStartConv(void);

#endif
