#include "extin.h"
#include "pages.h"

#warning "Old version"



void ExtinInit(void)
{
	GPIO_InitTypeDef gi;
	EXTI_InitTypeDef ei;
	NVIC_InitTypeDef ni;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	gi.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&gi);
	gi.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOE,&gi);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource1);
	
	ei.EXTI_Line=EXTI_Line0;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	ei.EXTI_Line=EXTI_Line1;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI0_IRQn;
	ni.NVIC_IRQChannelCmd=ENABLE;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&ni);
	ni.NVIC_IRQChannel=EXTI1_IRQn;
	NVIC_Init(&ni);
	
}

void EXTI0_IRQHandler(void)
{	
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line0);
		sys.intFlag[0]=1;
		sys.intTime[0]=sys.sensors.header.time;
		
	}	
}
void EXTI1_IRQHandler(void)
{	
	if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line1);		
		sys.intFlag[1]=1;
		sys.intTime[1]=sys.sensors.header.time;
		
	}	
}

//void EXTI15_10_IRQHandler(void)
//{	
//	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
//	{
//		sys.intFlag[0]=1;
//		//sys.sensors.SensorData[2]++;
//		intTempTime=sys.sensors.header.time;
//		sys.freq[0]=1000.0/(intTempTime-lastIntTime[0]);
//		lastIntTime[0]=intTempTime;
//		EXTI_ClearITPendingBit(EXTI_Line12);		
//	}
//	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
//	{
//		sys.intFlag[1]=1;
//		intTempTime=sys.sensors.header.time;
//		sys.freq[1]=1000.0/(intTempTime-lastIntTime[1]);
//		lastIntTime[1]=intTempTime;
//		EXTI_ClearITPendingBit(EXTI_Line13);
//	}
//	
//	
//}

