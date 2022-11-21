#ifndef __PAGES_H
#define __PAGES_H
#include "sys.h"
#include "keyboard.h"
#include "sblink.h"
#include "stdio.h"
#include "rtc.h"
#include "nickelbird.h"

#define MainPage 0
#define FalconPage 1
#define ControllerPage 2

#define PageNum 3



extern u8 lastKey;
extern u8 currKey;
extern s32 lastWheel;
extern s32 currWheel;
extern u8 lastWheelPush;
extern u8 currWheelPush;
extern u8 keyPress;
extern u8 wheelPress;

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

void PageInit_controller(u8 f);
void PageUpdate_controller(void);


//void PageInit_ADC(u8 f);
//void PageUpdate_ADC(void);

//void PageInit_Motor(u8 f);
//void PageUpdate_Motor(void);

#endif
