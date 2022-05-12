#include "timer.h"
#include "pages.h"

u32 lastCCR;
u32 EOsum;
u32 EOcnt;

void MainClockInit(void)
{				
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);	
	ti.TIM_Period = 9; //1ms
	ti.TIM_Prescaler =8400-1;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM7, &ti);
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM7, ENABLE);
	ni.NVIC_IRQChannel = TIM7_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 1; 
	ni.NVIC_IRQChannelSubPriority = 2;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);	
}

void PreciseClockInit(void)
{
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	TIM_ICInitTypeDef tc;
	GPIO_InitTypeDef gi;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_3;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;	
	GPIO_Init(GPIOB,&gi);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2);
	
	ti.TIM_Period = 0xFFFFFFFF; //1ms
	ti.TIM_Prescaler =84-1;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &ti);
	
	tc.TIM_Channel=TIM_Channel_2;
	tc.TIM_ICPolarity=TIM_ICPolarity_Falling;
	tc.TIM_ICSelection=TIM_ICSelection_DirectTI;
	tc.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	tc.TIM_ICFilter=0x0F;
	TIM_ICInit(TIM2,&tc);
	
	
	
	ni.NVIC_IRQChannel = TIM2_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 2; 
	ni.NVIC_IRQChannelSubPriority = 2;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);	
	
	TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE ); 
	TIM_Cmd(TIM2, ENABLE);
	
}

void TIM2_IRQHandler(void)
{
	u32 ccr,val;
	if(TIM_GetITStatus(TIM2,TIM_IT_CC2)!=RESET)
	{
		//printf("TIM2CC\r\n");
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);
		ccr=TIM_GetCapture2(TIM2);	
		if(ccr>=lastCCR)
		{
			val=ccr-lastCCR;
		}
		else
		{
			val=(0xFFFFFFFF-ccr)+lastCCR;
		}		
		lastCCR=ccr;
		EOsum+=val;
		EOcnt++;
		if(EOcnt>=40)
		{
			sys.EOtime=EOsum;
			EOsum=0;
			EOcnt=0;			
		}
		//TIM_ClearITPendingBit(TIM2,TIM_IT_CC3);
	}
	
	
}