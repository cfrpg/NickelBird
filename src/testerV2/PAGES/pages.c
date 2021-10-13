#include "pages.h"
#include "oled.h"
#include "keyboard.h"
#include "parameter.h"

void PagesInit(void)
{	
	const s32* t=ParamGetFromName("PWM_MIN");
	
	sys.adcEn=0;
	sys.pwm[0]=*t;
	sys.pwm[1]=*t;
	
	sys.sensors.header.stx=LINKSTX;
	sys.sensors.header.len=DataLen[SENSOR_DATA_V2];
	sys.sensors.header.fun=SENSOR_DATA_V2;	
	sys.sensors.header.time=0;
	sys.slowUpdate=0;
	sys.fastUpdate=0;
	sys.intUpdate=0;
	sys.intReset=0;
	
	PageInit_main(1);
	PageInit_falcon(1);
	//PageInit_ADC(1);
	currpage=-1;
}

void PagesChangeTo(u8 p)
{	
	if(p!=currpage)
	{
		currpage=p;
		switch(p)
		{
			case MainPage:
				PageInit_main(0);
			break;
			case FalconPage:
				PageInit_falcon(0);
			break;

			default:
				currpage=MainPage;
				PageInit_main(0);
			break;
		}
	}
	PagesDrawStatusBar();
}

void PagesNext(s8 d)
{	
	if(d<0)
	{
		if(currpage==0)
			PagesChangeTo(PageNum-1);
		else
			PagesChangeTo(currpage-1);
	}
	if(d>0)
		PagesChangeTo((currpage+1)%PageNum);
}

void PagesDrawStatusBar(void)
{	
//	RTCReadTime();
//	OledDispString(0,0,"  -  -     :  :      ",0);
//	OledDispInt(0,0,time.year,2,0);
//	OledDispInt(3,0,time.month,2,0);
//	OledDispInt(6,0,time.day,2,0);
//	OledDispInt(9,0,time.hour,2,0);
//	OledDispInt(12,0,time.min,2,0);
//	OledDispInt(15,0,time.sec,2,0);
}

void PagesUpdate(void)
{	
	currKey=GPGetData();
	currWheel=WheelGetValue();
	currWheelPush<<=1;
	currWheelPush|=KeyGetState();
	keyPress=currKey&(currKey^lastKey);
	wheelPress=((currWheelPush&0x07)==0x01);
	switch(currpage)
	{
		case MainPage:
			PageUpdate_main();
		break;
		case FalconPage:
			PageUpdate_falcon();
		break;
		default:
			currpage=MainPage;
			PageInit_main(0);
			PageUpdate_main();
		break;
	}
	lastKey=currKey;
	lastWheel=currWheel;
}

void PagesDrawHeader(u8 n,s8 *name)
{
	OledDispString(0,1,"=",0);
	OledDispString(19,1,"=",0);
	OledDispString(4,1,"/",0);
	OledDispInt(5,1,PageNum,1,0);
	if(n>9)
		OledDispInt(2,1,n+1,2,0);
	else
		OledDispInt(3,1,n+1,1,0);
	OledDispString(8,1,name,0);
}
