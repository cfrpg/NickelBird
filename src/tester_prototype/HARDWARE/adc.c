#include "adc.h"
#include "stdio.h"
#include "delay.h"

void ADCInit(void)
{
	GPIO_InitTypeDef gi;
	ADC_CommonInitTypeDef ac;
	ADC_InitTypeDef ai;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	gi.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2;
	gi.GPIO_Mode=GPIO_Mode_AN;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&gi);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);
	
	ac.ADC_Mode=ADC_Mode_Independent;
	ac.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;
	ac.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;
	ac.ADC_Prescaler=ADC_Prescaler_Div4;
	ADC_CommonInit(&ac);
	
	ai.ADC_Resolution=ADC_Resolution_12b;
	ai.ADC_ScanConvMode=DISABLE;
	ai.ADC_ContinuousConvMode=DISABLE;
	ai.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	ai.ADC_DataAlign=ADC_DataAlign_Right;
	ai.ADC_NbrOfConversion=1;
	ADC_Init(ADC1,&ai);
	ADC_Cmd(ADC1,ENABLE);	
}

void ADCReadVol(short res[])
{
	u32 tmp;
	u8 i,j;
	for(j=0;j<2;j++)
	{
		ADC_RegularChannelConfig(ADC1,11+j,1,ADC_SampleTime_56Cycles);
		tmp=0;
		for(i=0;i<16;i++)
		{
			ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
			
			ADC_SoftwareStartConv(ADC1);
			while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
			tmp+=ADC_GetConversionValue(ADC1)&0x0FFF;			
			//delay_ms(1);
		}
		tmp>>=4;
		res[j]=((short)tmp);
	}
	//printf("ad %d %f %f\r\n",tmp,res[0],res[1]);
}
