#ifndef __SBLINK_H
#define __SBLINK_H
#include "sys.h"
#include "rtc.h"

#define SENDBUFFSIZE 256
#define LINKSTX 0xFC

#define HEARTBEAT 0x00
#define SENSOR_DATA 0x01

extern const u8 DataLen[];
extern u8 recBuff[2][256];
extern u8 recBuffLen[2];
extern u8 currBuff;
extern u8 sblinkReady;

typedef struct
{
	u8 stx;
	u8 len;
	u8 seq;
	u8 fun;
	u32 time;
} PackageHeader;

typedef struct
{
	PackageHeader header;
	short ADCData[12];	
	float SensorData[4];
} SensorDataPackage;

typedef struct
{
	PackageHeader header;
	DateTime time;
} TimeDataPackage;

extern u8 sendBuff[SENDBUFFSIZE];

void LinkInit(void);
void LinkSendData(void* buff,u8 len);
u32 LinkPackTime(void);

#endif
