#include "eo.h"

void EOInit(void)
{
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	EXTI_InitTypeDef ei;
	GPIO_InitTypeDef gi;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);	
	ti.TIM_Period = 9999; //100ms
	ti.TIM_Prescaler =8400-1;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM12, &ti);
	TIM_ITConfig(TIM12,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM12, ENABLE);
	ni.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 2; 
	ni.NVIC_IRQChannelSubPriority = 3;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);	
	

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;	
	GPIO_Init(GPIOD,&gi);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource4);
	
	ei.EXTI_Line=EXTI_Line4;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI4_IRQn;
	ni.NVIC_IRQChannelCmd=ENABLE;
	ni.NVIC_IRQChannelPreemptionPriority=1;
	ni.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&ni);
}
