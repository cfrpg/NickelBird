#include "sampling.h"
#include "pages.h"

void SamplingInit(void)
{
	GPIO_InitTypeDef gi;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	// External trigger clock
	// Timer capture	
	gi.GPIO_Pin=GPIO_Pin_5;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9);
	GPIO_Init(GPIOE,&gi);	
	
	// External trigger enabled
	// GPIO input
	gi.GPIO_Pin=GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&gi);
	
	// External trigger inverse control
	// GPIO output
	// Set 1 to reverse trigger signal
	gi.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&gi);
	EXT_CLK_INV=0;
	EXT_EN_INV=0;
	
	// Sampling trigger select
	// GPIO output
	// Set 1 to use external trigger signal
	gi.GPIO_Pin=GPIO_Pin_2;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&gi);
	SPL_TRIG_SEL=0;
	
	// Internal clock
	// Timer PWM output
	gi.GPIO_Pin=GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);
	GPIO_Init(GPIOC,&gi);
	
	// Internal clock timer
	// TIM3_CH1
	TIM_TimeBaseInitTypeDef ti;
	TIM_OCInitTypeDef to;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
	
	ti.TIM_Period = 84-1; //1ms
	ti.TIM_Prescaler =999;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &ti);
	
	to.TIM_OCMode=TIM_OCMode_PWM1;
	to.TIM_OutputState=TIM_OutputState_Enable;
	to.TIM_Pulse=1;
	to.TIM_OCPolarity=TIM_OCPolarity_Low;
	
	TIM_OC1Init(TIM3,&to);
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	
	
	// TIM3 INT	
//	NVIC_InitTypeDef ni;
//	ni.NVIC_IRQChannel = TIM3_IRQn;
//	ni.NVIC_IRQChannelPreemptionPriority = 1; 
//	ni.NVIC_IRQChannelSubPriority = 1;
//	ni.NVIC_IRQChannelCmd = ENABLE;	
//	NVIC_Init(&ni);		
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM3, ENABLE);
	
	// External clock timer
	// TIM9_CH1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	ti.TIM_Period = 168-1; //1us
	ti.TIM_Prescaler =999;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM9, &ti);
	
	to.TIM_OCMode=TIM_OCMode_PWM1;
	to.TIM_OutputState=TIM_OutputState_Enable;
	to.TIM_Pulse=1;
	to.TIM_OCPolarity=TIM_OCPolarity_Low;
	
	TIM_OC1Init(TIM9,&to);
	TIM_OC1PreloadConfig(TIM9,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM9,ENABLE);
	//TIM_Cmd(TIM9, ENABLE);
}

void SamplingSlowUpdate(u16 time)
{	
	if(sys.slowUpdate!=0)
	{
		(*sys.slowUpdate)(time);
	}
}

void SamplingFastUpdate(u16 time)
{
	if(sys.fastUpdate!=0)
	{
		(*sys.fastUpdate)(time);
	}
}

void SamplingIntUpdate(void)
{
	if(sys.intUpdate!=0)
	{
		(*sys.intUpdate)();
	}
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//TEST1=1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if(sys.adcClkSource==INTERNAL&&(!sys.adcBusy))
		{
			sys.adcBusy=1;
			sys.sensors.header.time++;
		}		
		//TEST0=0;
	}
}
