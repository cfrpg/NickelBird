#include "dac.h"

void DACInit(void)
{
	GPIO_InitTypeDef gi;
	DAC_InitTypeDef di;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4;
	gi.GPIO_Mode=GPIO_Mode_AN;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA,&gi);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	
	di.DAC_Trigger=DAC_Trigger_None;
	di.DAC_WaveGeneration=DAC_WaveGeneration_None;
	di.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
	di.DAC_OutputBuffer=DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1,&di);
	
	
	DAC_Cmd(DAC_Channel_1,ENABLE);	
	
	DACSet(0);
}

void DACSet(float val)
{
	u16 v=1<<12;
	v=v*val;	
	DAC_SetChannel1Data(DAC_Align_12b_R,v);	
}
