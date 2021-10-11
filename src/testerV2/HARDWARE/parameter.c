#include "parameter.h"
#include "mb85rs.h"
#include "stdio.h"
#include "string.h"

ParameterSet params;

u32 param_readWord(u32 addr)
{
	return *(vu32*)addr;
}


u8 ParamRead(void)
{
	FRAMRead(0,sizeof(params),(u8*)(&params));
	return 0;
}


u8 ParamWrite(void)
{	
	FRAMWrite(0,sizeof(params),(u8*)(&params));
	return 0;
}

void ParamReset(void)
{
	u8 i;
	params.headFlag=0xCFCF;
	for(i=0;i<PARAM_NUM;i++)
	{
		if(parameterList[i].type>0)
			params.values[i].floatValue=parameterList[i].defaultValue.floatValue;
		if(parameterList[i].type<=0)
			params.values[i].intValue=parameterList[i].defaultValue.intValue;
	}
	params.tailFlag=0xFCFC;
}

float paramReadFixed(s32 v,u8 n)
{
	float res=v;
	while(n--)
	{
		res/=10;
	}
	return res;
}
u32 paramReadBinary(s32 v)
{
	u32 res=0;
	u32 mask=1;
	while(v>0)
	{
		if(v%10==1)
		{
			res|=mask;
		}
		mask<<=1;
		v/=10;
	}
	return res;
}

u8 ParamSet(u8 id,s32 v)
{
	s8 type;
	
	if(id>=PARAM_NUM)
		return 1;
	type=parameterList[id].type;
	if(type>0)
	{
		params.values[id].floatValue=paramReadFixed(v,type);
		if(params.values[id].floatValue>parameterList[id].maxValue.floatValue)
		{
			params.values[id].floatValue=parameterList[id].maxValue.floatValue;
		}
		if(params.values[id].floatValue<parameterList[id].minValue.floatValue)
		{
			params.values[id].floatValue=parameterList[id].minValue.floatValue;
		}
	}
	else
	{
		if(type==0)
		{
			params.values[id].intValue=v;
		}
		else
		{
			params.values[id].intValue=paramReadBinary(v);
		}
		if(params.values[id].intValue>parameterList[id].maxValue.intValue)
		{
			params.values[id].intValue=parameterList[id].maxValue.intValue;
		}
		if(params.values[id].intValue<parameterList[id].minValue.intValue)
		{
			params.values[id].intValue=parameterList[id].minValue.intValue;
		}
	}
	
	return ParamWrite();
}

void ParamShow(void)
{
	u8 i;
	for(i=0;i<PARAM_NUM;i++)
	{
		if(parameterList[i].type<=0)
		{
			printf("#%d:%s(%d):%d\r\n",i,parameterList[i].name,parameterList[i].type,params.values[i].intValue);
		}
		if(parameterList[i].type>0)
		{
			printf("#%d:%s(%d):%f\r\n",i,parameterList[i].name,parameterList[i].type,params.values[i].floatValue);
		}		
	}
	printf("Param end.\r\n");
}

void* ParamGetFromName(const char* name)
{
	//find parameter id
	u8 flag=0;
	u8 i=0;	
	for(i=0;i<PARAM_NUM;i++)
	{
		if(strcmp(name,parameterList[i].name)==0)
		{
			flag=1;			
			break;
		}
	}
	if(flag)
	{
		if(parameterList[i].type==0)
		{
			return &(params.values[i].intValue);
		}
		if(parameterList[i].type<0)
		{
			return &(params.values[i].intValue);
		}
		if(parameterList[i].type>0)
		{
			return &(params.values[i].floatValue);
		}
	}
	return 0;
}

u8 ParamSetWithName(const char* name,void* value)
{
	u8 flag=0;
	u8 i=0;	
	for(i=0;i<PARAM_NUM;i++)
	{
		if(strcmp(name,parameterList[i].name)==0)
		{
			flag=1;			
			break;
		}
	}
	if(flag)
	{
		if(parameterList[i].type<=0)
		{
			params.values[i].intValue=*((s32*)value);
			if(params.values[i].intValue>parameterList[i].maxValue.intValue)
			{
				params.values[i].intValue=parameterList[i].maxValue.intValue;
			}
			if(params.values[i].intValue<parameterList[i].minValue.intValue)
			{
				params.values[i].intValue=parameterList[i].minValue.intValue;
			}
				
		}
		if(parameterList[i].type>0)
		{
			params.values[i].floatValue=*((float*)value);
			if(params.values[i].floatValue>parameterList[i].maxValue.floatValue)
			{
				params.values[i].floatValue=parameterList[i].maxValue.floatValue;
			}
			if(params.values[i].floatValue<parameterList[i].minValue.floatValue)
			{
				params.values[i].floatValue=parameterList[i].minValue.floatValue;
			}
		}
		
	}
	return ParamWrite();
}
