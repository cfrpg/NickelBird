#ifndef __SENSORS_H
#define __SENSORS_H
#include "sys.h"



void SeneorsInit(void);
void SensorsSlowUpdate(u16 time);
void SensorsFastUpdate(u16 time);
void SensorsIntUpdate(void);

#endif
