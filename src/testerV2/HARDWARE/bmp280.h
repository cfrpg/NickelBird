#ifndef __BMP280_H
#define __BMP280_H
#include "sys.h"

#define BMP280_CS PGout(6)

#define BMP_Mode_Sleep	0x00
#define BMP_Mode_Forced	0x01
#define BMP_Mode_Normal	0x03

#define BMP_OS_Skip		0x00
#define BMP_OS_1		0x01
#define BMP_OS_2		0x02
#define BMP_OS_4		0x03
#define BMP_OS_8		0x04
#define BMP_OS_16		0x05

#define BMP_Tsb_Halfms	0x00
#define BMP_Tsb_62ms5	0x01
#define BMP_Tsb_125		0x02
#define BMP_Tsb_250		0x03
#define BMP_Tsb_500		0x04
#define BMP_Tsb_1000	0x05
#define BMP_Tsb_2000	0x06
#define BMP_Tsb_4000	0x07

void BMPInit(void);
void BMPConfigOS(u8 t,u8 p,u8 m);
void BMPConfigRate(u8 t,u8 iir);
float BMPReadPressure(void);
float BMPReadTemperature(void);
void BMPReadCali(void);
float BMPReadAverageRho(u8 n);

#endif
