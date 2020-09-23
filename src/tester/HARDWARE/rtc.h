#ifndef __RTC_H
#define __RTC_H
#include "sys.h"
typedef struct
{
	u16 ms;
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;
} DateTime;

extern DateTime time;
extern u16 RTCcnt;

u8 RTCInit(void);
ErrorStatus RTCSetTime(u8 h,u8 m,u8 s);
ErrorStatus RTCSetDate(u8 y,u8 m,u8 d);
void RTCReadTime(void);


#endif
