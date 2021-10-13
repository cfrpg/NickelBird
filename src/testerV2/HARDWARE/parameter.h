#ifndef __PARAMETER_H
#define __PARAMETER_H
#include "sys.h"

#define PARAM_TYPE_INT 0
#define PARAM_TYPE_FLOAT 1
#define PARAM_TYPE_BINARY 2

#define PARAM_NUM 10

typedef union
{
	s32 intValue;
	float floatValue;
} UnknowType;

typedef struct
{
	s32 intValue;
	float floatValue;
} BothType;

typedef struct
{
	s8 type;
	const char name[19];
	UnknowType defaultValue;	
	UnknowType minValue;	
	UnknowType maxValue;
} ParameterRecord;

typedef struct
{
	u16 headFlag;
	BothType values[PARAM_NUM];
	u16 tailFlag;
} ParameterSet;

extern const ParameterRecord parameterList[PARAM_NUM];

extern ParameterSet params;

u8 ParamRead(void);
u8 ParamWrite(void);
void ParamReset(void);
u8 ParamSet(u8 id,s32 v);
void ParamShow(void);
void* ParamGetFromName(const char* name);
u8 ParamSetWithName(const char* name,void* value);

#endif
