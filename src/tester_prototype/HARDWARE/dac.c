#include "dac.h"

void DACInit(void)
{
	GPIO_InitTypeDef gi;
	DAC_InitTypeDef di;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	gi.GPIO_Mode=GPIO_Mode_AN;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA,&gi);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	
	di.DAC_Trigger=DAC_Trigger_None;
	di.DAC_WaveGeneration=DAC_WaveGeneration_None;
	di.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
	di.DAC_OutputBuffer=DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1,&di);
	DAC_Init(DAC_Channel_2,&di);
	
	DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_Cmd(DAC_Channel_2,ENABLE);
	
	DACSet(1,0);
	DACSet(2,0);
}

void DACSet(u8 ch,float val)
{
	u16 v=1<<12;
	v=v*val;
	if(ch==1)
	{
		DAC_SetChannel1Data(DAC_Align_12b_R,v);
	}
	if(ch==2)
	{
		DAC_SetChannel2Data(DAC_Align_12b_R,v);
	}
}
