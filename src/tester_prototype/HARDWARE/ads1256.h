#ifndef __ADS1256_H
#define __ADS1256_H
#include "sys.h"

#define ADS_CS PDout(1)
#define ADS_SYNC PDout(3)
#define ADS_RST PDout(2)
#define ADS_DRDY PDin(0)
#define ADS_DI PCout(12)
#define ADS_DO PCin(11)
#define ADS_SCK PCout(10)

#define ADS1256_GAIN_1 0	//增益1
#define ADS1256_GAIN_2 1	//增益2
#define ADS1256_GAIN_4 2	//增益4
#define ADS1256_GAIN_8 3	//增益8
#define ADS1256_GAIN_16 4	//增益16
#define ADS1256_GAIN_32 5	//增益32
#define ADS1256_GAIN_64 6	//增益64

#define ADS1256_30000SPS 0xF0
#define ADS1256_15000SPS 0xE0
#define ADS1256_7500SPS 0xD0
#define ADS1256_3750SPS 0xC0
#define ADS1256_2000SPS 0xB0
#define ADS1256_1000SPS 0xA1
#define ADS1256_500SPS 0x92
#define ADS1256_100SPS 0x82
#define ADS1256_60SPS 0x72
#define ADS1256_50SPS 0x63
#define ADS1256_30SPS 0x53
#define ADS1256_25SPS 0x43
#define ADS1256_15SPS 0x33
#define ADS1256_10SPS 0x20
#define ADS1256_5SPS 0x13
#define ADS1256_2d5SPS 0x03

#define ADS_CMD_WAKEUP   0x00 // Completes SYNC and Exits Standby Mode 0000  0000 (00h)
#define ADS_CMD_RDATA    0x01 // Read Data 0000  0001 (01h)
#define ADS_CMD_RDATAC   0x03 // Read Data Continuously 0000   0011 (03h)
#define ADS_CMD_SDATAC   0x0F // Stop Read Data Continuously 0000   1111 (0Fh)
#define ADS_CMD_RREG     0x10 // Read from REG rrr 0001 rrrr (1xh)
#define ADS_CMD_WREG     0x50 // Write to REG rrr 0101 rrrr (5xh)
#define ADS_CMD_SELFCAL  0xF0 // Offset and Gain Self-Calibration 1111    0000 (F0h)
#define ADS_CMD_SELFOCAL 0xF1 // Offset Self-Calibration 1111    0001 (F1h)
#define ADS_CMD_SELFGCAL 0xF2 // Gain Self-Calibration 1111    0010 (F2h)
#define ADS_CMD_SYSOCAL  0xF3 // System Offset Calibration 1111   0011 (F3h)
#define ADS_CMD_SYSGCAL  0xF4 // System Gain Calibration 1111    0100 (F4h)
#define ADS_CMD_SYNC     0xFC // Synchronize the A/D Conversion 1111   1100 (FCh)
#define ADS_CMD_STANDBY  0xFD // Begin Standby Mode 1111   1101 (FDh)
#define ADS_CMD_RESET    0xFE // Reset to Power-Up Values 1111   1110 (FEh)

#define ADS_REG_STATUS  0 // x1H
#define ADS_REG_MUX     1 // 01H
#define ADS_REG_ADCON   2 // 20H
#define ADS_REG_DRATE   3 // F0H
#define ADS_REG_IO      4 // E0H
#define ADS_REG_OFC0    5 // xxH
#define ADS_REG_OFC1    6 // xxH
#define ADS_REG_OFC2    7 // xxH
#define ADS_REG_FSC0    8 // xxH
#define ADS_REG_FSC1    9 // xxH
#define ADS_REG_FSC2    10 // xxH

extern u16 cpucnt;
extern u16 ADSCpu2;

void ADS1256Init(void);
s32 ADSReadChannel(u8 ch);
void ADSStartUp(u8 gain,u8 rate);
void ADSSetChannel(u8 ch);
s32 ADSReadCurrChannel(void);
void ADSReadAllChannel(s32 data[]);

#endif
