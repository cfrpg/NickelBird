#include "pwm.h"
#include "pages.h"

u8 _pwm_armed;

void PWMInit(void)
{
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef ti;
	TIM_OCInitTypeDef to;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3);

	
	gi.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gi);
	GPIO_Init(GPIOB,&gi);
	
	ti.TIM_Prescaler=84-1;
	ti.TIM_Period=19999;
	ti.TIM_ClockDivision=TIM_CKD_DIV1;
	ti.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5,&ti);
	TIM_TimeBaseInit(TIM3,&ti);
	
	to.TIM_OCMode=TIM_OCMode_PWM1;
	to.TIM_OutputState=TIM_OutputState_Enable;
	to.TIM_OCPolarity=TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM5,&to);
	TIM_OC2Init(TIM5,&to);
	TIM_OC3Init(TIM3,&to);
	TIM_OC4Init(TIM3,&to);

	
	TIM_OC1PreloadConfig(TIM5,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM5,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM5,ENABLE);
	TIM_Cmd(TIM5,ENABLE);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	PWMDisarm();
}

void PWMSet(u32 ch0,u32 ch1)
{
	if(_pwm_armed)
	{			
		TIM_SetCompare1(TIM5,ch0);
		TIM_SetCompare2(TIM5,ch1);	
		sys.pwm[0]=ch0;
		sys.pwm[1]=ch1;
	}
}

void PWMSetAux(u32 ch0,u32 ch1)
{	
		TIM_SetCompare3(TIM3,ch0);
		TIM_SetCompare4(TIM3,ch1);	
		sys.pwm[2]=ch0;
		sys.pwm[3]=ch1;
	
}

void PWMDisarm(void)
{
	_pwm_armed=0;
//	TIM_SetCompare1(TIM5,params.pwm_disarmed);
//	TIM_SetCompare2(TIM5,params.pwm_disarmed);	
	TIM_SetCompare1(TIM5,1000);
	TIM_SetCompare2(TIM5,1000);	
}

void PWMArm(void)
{
	_pwm_armed=1;	
}

u8 PWMIsArmed(void)
{
	return _pwm_armed;
}
