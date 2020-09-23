#ifndef __OLED_H
#define __OLED_H
#include "sys.h"

#define OLED_CS PBout(12)
#define OLED_DC PBout(14)
#define OLED_RST PDout(8)
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
void OledSetChar(u8 x,u8 y,s8 c,u8 f);
u8 OledSetString(u8 x,u8 y,s8 c[],u8 f);
void OledSetRev(u8 x,u8 y,s8 len,u8 f);

void OledRefreshPart(u8 x,u8 y,s8 len);
void OledDispChar(u8 x,u8 y,s8 c,u8 f);
void OledDispString(u8 x,u8 y,s8 c[],u8 f);
void OledDispInt(u8 x,u8 y,s32 v,u8 ml,u8 f);
void OledDispFloat(u8 x,u8 y,float v,s8 len,s8 pre,u8 f);
void OledDispFixed(u8 x,u8 y,s32 v,s8 pre,u8 ml,u8 f);
#endif
