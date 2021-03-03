#include "adc.h"
#include "stdio.h"
#include "delay.h"

volatile u16 adc_buff[8][4];
u16 adc_res[4];
u8 aux_adc_ready;

void ADCInit(void)
{
	GPIO_InitTypeDef gi;
	ADC_CommonInitTypeDef ac;
	ADC_InitTypeDef ai;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	gi.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	gi.GPIO_Mode=GPIO_Mode_AN;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&gi);
		
	ac.ADC_Mode=ADC_Mode_Independent;
	ac.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_10Cycles;
	ac.ADC_DMAAccessMode=ADC_DMAAccessMode_1;
	ac.ADC_Prescaler=ADC_Prescaler_Div4;
	ADC_CommonInit(&ac);
	
	ai.ADC_Resolution=ADC_Resolution_12b;
	ai.ADC_ScanConvMode=ENABLE;
	ai.ADC_ContinuousConvMode=ENABLE;
	ai.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	ai.ADC_DataAlign=ADC_DataAlign_Right;
	ai.ADC_NbrOfConversion=4;
	ADC_Init(ADC1,&ai);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,2,ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_12,3,ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_13,4,ADC_SampleTime_28Cycles);
	
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);	
	
	DMA_InitTypeDef di;
	NVIC_InitTypeDef ni;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
	di.DMA_Channel=DMA_Channel_0;
	di.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;
	di.DMA_Memory0BaseAddr=(u32)adc_buff;
	di.DMA_DIR=DMA_DIR_PeripheralToMemory;
	di.DMA_BufferSize=4*8;
	di.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	di.DMA_MemoryInc=DMA_MemoryInc_Enable;
	di.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	di.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	di.DMA_Mode=DMA_Mode_Circular;
	di.DMA_Priority=DMA_Priority_Medium;
	di.DMA_FIFOMode=DMA_FIFOMode_Disable;
	di.DMA_FIFOThreshold=DMA_FIFOThreshold_Full;
	di.DMA_MemoryBurst=DMA_MemoryBurst_Single;
	di.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
	
	DMA_Init(DMA2_Stream0,&di);
	DMA_ClearFlag(DMA2_Stream0,DMA_IT_TC);
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);
	
	ni.NVIC_IRQChannel=DMA2_Stream0_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=0x01;
	ni.NVIC_IRQChannelSubPriority=0x01;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);
	//DMA_SetCurrDataCounter(DMA2_Stream0,4*16);
	DMA_Cmd(DMA2_Stream0,ENABLE);	
	ADC_SoftwareStartConv(ADC1);
	
	aux_adc_ready=1;
}

void ADCReadVol(short res[])
{
	u8 i;
	res[0]=0;
	res[1]=0;
	res[2]=0;
	res[3]=0;
	for(i=0;i<8;i++)
	{
		res[0]+=adc_buff[i][0];
		res[1]+=adc_buff[i][1];
		res[2]+=adc_buff[i][2];
		res[3]+=adc_buff[i][3];
	}
	res[0]>>=4;
	res[1]>>=4;
	res[2]>>=4;
	res[3]>>=4;
}

void ADCStartConv()
{
	
	while(aux_adc_ready==0);
	aux_adc_ready=0;
	//printf("a\r\n");
	//while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);
	//printf("b\r\n");
	ADC_Cmd(ADC1,ENABLE);
	//DMA_Cmd(DMA2_Stream0,ENABLE);	
	//printf("c\r\n");
	ADC_SoftwareStartConv(ADC1);
}

void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream0,DMA_IT_TCIF0)==SET)
	{
		//printf("adc dma\r\n");
		aux_adc_ready=1;		
		DMA_ClearFlag(DMA2_Stream0,DMA_IT_TCIF0);
		ADC_Cmd(ADC1,DISABLE);
		//DMA_Cmd(DMA2_Stream0,DISABLE);
		
	}
}
