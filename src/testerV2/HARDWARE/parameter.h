#ifndef __PARAMETER_H
#define __PARAMETER_H
#include "sys.h"

#define TGT_ADDR ((u32)0x000E0000)
#define TGT_SECTOR FLASH_Sector_11

typedef struct
{
	u32 headFlag;	
	u32 version;
	u32 pwm_min;
	u32 pwm_max;
	u32 pwm_disarmed;
	u32 tailFlag;
} ParameterSet;

extern ParameterSet params;

u8 ParamRead(void);
FLASH_Status ParamWrite(void);
void ParamReset(void);

#endif
