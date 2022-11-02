#ifndef __OLED_H
#define __OLED_H
#include "sys.h"

#define OLED_CS PCout(5)
#define OLED_DC PAout(6)
#define OLED_RST PCout(4)
#define OLED_CMD 0
#define OLED_DATA 1

void OledInit(void);
void OledStartUp(void);
void OledSendByte(u8 b);
void OledDispOn(void);
void OledDispOff(void);
void OledSetPos(u8 x,u8 y);
void OledClear(u8 c);
void OledClearBuff(void);
void OledRefresh(void);

// Full screen refresh time: 3.12ms
void OledStartRefresh(void);

// Draw benchmark
// String: 	16Line * 21Char: 	265us
// Bitmap: 	128 * 128: 			3.92ms
// Int:  	32Line * 10Digit: 	375us
// Fixed: 	32Line * 10Digit: 	385us
// Fixed: 	32Line * 7Digit: 	245us

//void OledRefreshPart(u8 x,u8 y,s8 len);
//void OledDispChar(u8 x,u8 y,s8 c,u8 f);
//void OledDispString(u8 x,u8 y,s8 c[],u8 f);
void OledDrawChar(u8 x,u8 y,s8 c,u8 f);
u8 OledDrawString(u8 x,u8 y,s8 c[],u8 f);
void OledDrawInt(u8 x,u8 y,s32 v,u8 ml,u8 f);
void OledDrawFloat(u8 x,u8 y,float v,s8 len,s8 pre,u8 f);
void OledDrawFixed(u8 x,u8 y,s32 v,s8 pre,u8 ml,u8 f);
void OledDrawBitmap(u8 x,u8 y,u8 w,u8 h,const u8* data);

void OledSetStringRev(u8 x,u8 y,s8 len,u8 f);

#endif
