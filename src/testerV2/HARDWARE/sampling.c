#include "sampling.h"
#include "pages.h"
#include "ad7606fsmc.h"
#include "sblink.h"
#include "nickelbird.h"
#include "parameter.h"

u32 splcnt;
u32 splsum;
u32 spllast;
u32 _freq_backup;

SamplingState splState;

void SamplingInit(void)
{
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef ti;
	TIM_OCInitTypeDef to;
	NVIC_InitTypeDef ni;
	EXTI_InitTypeDef ei;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	// Get parameters
	splState.defaultFreq=ParamGetFromName("SPL_CLK_RATE");
	splState.defaultClkPolarity=ParamGetFromName("SPL_ECLK_POL");
	splState.defaultEnablePolarity=ParamGetFromName("SPL_EN_POL");
	
	splState.ExternalClockFrequency=0;
	splState.InternalClockFrequency=1000;
	splState.ExternalEnabledPolarity=Active_High;
	splState.ExternalClockPolarity=Failing;
	splState.ClockSource=INTERNAL;
	
	splState.adcBusy=0;
	
	
//Initialize signal control
	// External trigger inverse control
	// Set 1 to reverse trigger signal
	gi.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&gi);
	EXT_CLK_INV=(u8)(*splState.defaultClkPolarity)&0x01;
	EXT_EN_INV=(u8)(*splState.defaultEnablePolarity)&0x01;
	
	// Sampling trigger select
	// Set 1 to use external trigger signal
	gi.GPIO_Pin=GPIO_Pin_2;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&gi);
	SPL_TRIG_SEL=0;	
	
//Initialize internal sampling clock
	// Timer PWM output GPIO
	gi.GPIO_Pin=GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	GPIO_Init(GPIOC,&gi);
	
	// Internal clock timer
	// TIM8_CH1	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	
	
	ti.TIM_Period = 999; //default 1ms=1000Hz
	ti.TIM_Period = (1000000/(*splState.defaultFreq))&0xFFFF;
	ti.TIM_Prescaler =168-1;//1us
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM8, &ti);
	
	to.TIM_OCMode=TIM_OCMode_PWM1;
	to.TIM_OutputState=TIM_OutputState_Enable;
	to.TIM_Pulse=50;
	to.TIM_OCPolarity=TIM_OCPolarity_Low;
	
	TIM_OC1Init(TIM8,&to);
	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM8,ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	
	// Internal clock sampling interrupt	
	ni.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 1; 
	ni.NVIC_IRQChannelSubPriority = 1;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);		
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM8, ENABLE);	
	
	
// Initialize external sampling clock	
	// Timer capture GPIO	
	gi.GPIO_Pin=GPIO_Pin_5;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9);
	GPIO_Init(GPIOE,&gi);
	
	// External clock timer
	// TIM9_CH1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	ti.TIM_Period = 0xFFFF;//65ms=15Hz
	ti.TIM_Prescaler =168-1;//1us
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM9, &ti);

	TIM_ICInitTypeDef tic;
	tic.TIM_Channel=TIM_Channel_1;
	tic.TIM_ICFilter=0x00;
	tic.TIM_ICPolarity=TIM_ICPolarity_Falling;
	tic.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	tic.TIM_ICSelection=TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM9,&tic);

	TIM_ITConfig(TIM9,TIM_IT_CC1,ENABLE);
	ni.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=1;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	TIM_ARRPreloadConfig(TIM9,ENABLE);
	TIM_Cmd(TIM9, ENABLE);
	
	// Enable input GPIO
	gi.GPIO_Pin=GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&gi);
	
	// Enable input interrupt
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource6);
	ei.EXTI_Line=EXTI_Line6;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	ni.NVIC_IRQChannel=EXTI9_5_IRQn;
	ni.NVIC_IRQChannelCmd=ENABLE;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&ni);
	
	splcnt=0;
	splsum=0;
	_freq_backup=(u16)(*splState.defaultFreq);
	sys.sensors.Frequency=(u16)(*splState.defaultFreq);
	sys.sensors.Config|=0b10000000;
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
	AD7606FSMCRead(sys.sensors.ADCData);
	if(sys.intUpdate!=0)
	{
		(*sys.intUpdate)();
	}
	LinkSendData(&sys.sensors,sizeof(SensorDataPackage));
	EXTI_ClearITPendingBit(EXTI_Line13);
	splState.adcBusy=0;
	if(sys.intReset!=0)
	{
		(*sys.intReset)();
	}
}

void SamplingSetInternalFrequency(u32 f)
{
	TIM8->ARR=(1000000/f)&0xFFFF;
	sys.sensors.Frequency=(u16)f;
}

void SamplingSetExternalClockPolarity(u8 p)
{
	splState.ExternalClockPolarity=p;
	EXT_CLK_INV=p;
}

void SamplingSetExternalEnabledPolarity(u8 p)
{
	splState.ExternalEnabledPolarity=p;
	EXT_EN_INV=p;
}

void SamplingSetClockSource(u8 t)
{
	if(t==INTERNAL)
	{
		//stop external clock
		SPL_TRIG_SEL=INTERNAL;		
		//wait ADC
		while(splState.adcBusy);
		//start internal clock
		splState.ClockSource=INTERNAL;
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		TIM_Cmd(TIM8, ENABLE);
		// Recover sampling frequency
		sys.sensors.Frequency=_freq_backup;
		sys.sensors.Config|=0b10000000;
	}
	else
	{
		// Backup sampling frequency
		_freq_backup=sys.sensors.Frequency;
		//stop internal clock
		TIM_Cmd(TIM8, DISABLE);
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		splsum=0;
		splcnt=0;
		//wait adc
		//while(splState.adcBusy);
		//start external clock
		splState.ClockSource=EXTERNAL;
		SamplingCheckEnabled();	
		SPL_TRIG_SEL=EXTERNAL;
		
	}
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		//TEST1=1;
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		if(splState.ClockSource==INTERNAL&&(!splState.adcBusy))
		{
			splState.adcBusy=1;
			sys.sensors.header.time++;
		}		
		//TEST0=0;
	}
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
	u32 t;
	if(TIM_GetITStatus(TIM9,TIM_IT_CC1)!=RESET)
	{
		
		TIM_ClearITPendingBit(TIM9,TIM_IT_CC1);
		if(splState.ClockSource==EXTERNAL&&(!splState.adcBusy))
		{
			splState.adcBusy=1;
			sys.sensors.header.time++;
		}	
		t=TIM_GetCapture1(TIM9);
		if(splState.ClockSource==INTERNAL)
			return;
		if(t<spllast)
			splsum+=t+0xFFFF-spllast;
		else
			splsum+=t-spllast;
		spllast=t;
		splcnt++;
		if(splcnt==100)
		{
			if(splsum>=199995)
				splState.ExternalClockFrequency=100000000/splsum;
			else
				splState.ExternalClockFrequency=10000000/splsum*10;			
			splcnt=0;
			splsum=0;
			sys.sensors.Frequency=splState.ExternalClockFrequency;
		}
	}
}
void SamplingCheckEnabled(void)
{
	if(splState.ClockSource==INTERNAL)
		return;
	if(EXT_TRIG_EN)
	{
		sys.sensors.Config|=0b10000000;
	}
	else
	{
		sys.sensors.Config&=0b01111111;
	}
}

void EXTI9_5_IRQHandler(void)
{	
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line6);
		SamplingCheckEnabled();	
	}	
}
