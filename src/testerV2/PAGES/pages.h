#ifndef __PAGES_H
#define __PAGES_H
#include "sys.h"
#include "keyboard.h"
#include "sblink.h"
#include "stdio.h"
#include "rtc.h"

#define MainPage 0
#define FalconPage 1

#define PageNum 2

typedef struct
{
	s32 ADCRawData[8];
	float AuxADCData[2];
	SensorDataPackage sensors;
	s32 rpm;
	u16 pwm[2];
	u8 adcEn;
	float temperature;
	float pressure;
	float rho;
	float airspeed;
	float freq[2];
	void (*slowUpdate)(void);
	void (*fastUpdate)(void);
	void (*intUpdate)(void);
	u8 intFlag[2];
	u32 intTime[2];

} systemState;

extern systemState sys;
extern u8 lastKey;
extern u8 currKey;
extern s32 lastWheel;
extern s32 currWheel;
extern u8 lastWheelPush;
extern u8 currWheelPush;

extern s8 currpage;
extern u16 cpucnt;

void PagesInit(void);
void PagesChangeTo(u8 p);
void PagesNext(s8 d);
void PagesUpdate(void);
void PagesDrawHeader(u8 n,s8 *name);
void PagesDrawStatusBar(void);

void UpdateSensors(void);

void PageInit_main(u8 f);
void PageUpdate_main(void);

void PageInit_falcon(u8 f);
void PageUpdate_falcon(void);


//void PageInit_ADC(u8 f);
//void PageUpdate_ADC(void);

//void PageInit_Motor(u8 f);
//void PageUpdate_Motor(void);

#endif
