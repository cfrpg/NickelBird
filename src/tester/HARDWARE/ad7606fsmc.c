#include "ad7606fsmc.h"
#include "sblink.h"

void AD7606FSMCInit(void)
{
	GPIO_InitTypeDef gi;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	
	//common GPIO	
	//CV
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);
	gi.GPIO_Pin=GPIO_Pin_0;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gi);
	//CS
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_Pin=GPIO_Pin_13;
	GPIO_Init(GPIOC,&gi);
	//RST
	gi.GPIO_Pin=GPIO_Pin_11;
	GPIO_Init(GPIOF,&gi);
	//BUSY
	gi.GPIO_Pin=GPIO_Pin_2;
	gi.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(GPIOB,&gi);
	
	ADIF_CS=1;
	
	//FSMC GPIO
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//D3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);//NOE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);//NWE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);//D13
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);//D14
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);//D15
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);//D0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);//D1

	gi.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	gi.GPIO_Mode = GPIO_Mode_AF;
	gi.GPIO_Speed = GPIO_Speed_100MHz;
	gi.GPIO_OType = GPIO_OType_PP;
	gi.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &gi);
	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);//A20
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);//A21

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);//D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);//D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);//D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);//D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);//D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);//D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);//D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);//D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);//D12

	gi.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	GPIO_Init(GPIOE, &gi);
	
	//FSMC Config
	FSMC_NORSRAMInitTypeDef  fi;
	FSMC_NORSRAMTimingInitTypeDef  timing;
	
	timing.FSMC_AddressSetupTime = 3;
	timing.FSMC_AddressHoldTime = 0;
	timing.FSMC_DataSetupTime = 6;
	timing.FSMC_BusTurnAroundDuration = 1;
	timing.FSMC_CLKDivision = 0;
	timing.FSMC_DataLatency = 0;
	timing.FSMC_AccessMode = FSMC_AccessMode_A;
	
	fi.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	fi.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	fi.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	fi.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	fi.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	fi.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	fi.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	fi.FSMC_WrapMode = FSMC_WrapMode_Disable;
	fi.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	fi.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	fi.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	fi.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	fi.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	fi.FSMC_ReadWriteTimingStruct = &timing;
	fi.FSMC_WriteTimingStruct = &timing;

	FSMC_NORSRAMInit(&fi);
	
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
	
	AD7606FSMCReset();
	
	//TIM3_CH3
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
	
	TIM_OC3Init(TIM3,&to);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	
	
	//TIM3 INT	
	NVIC_InitTypeDef ni;
	ni.NVIC_IRQChannel = TIM3_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 1; 
	ni.NVIC_IRQChannelSubPriority = 1;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);		
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM3, ENABLE);
	//PB2 EXTI
	EXTI_InitTypeDef ei;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource2);
	
	ei.EXTI_Line=EXTI_Line2;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI2_IRQn;
	ni.NVIC_IRQChannelCmd=ENABLE;
	ni.NVIC_IRQChannelPreemptionPriority=0;
	ni.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&ni);
}

void AD7606FSMCReset(void)
{
	ADIF_RST=0;
	ADIF_RST=1;
	ADIF_RST=1;
	ADIF_RST=1;
	ADIF_RST=1;
	ADIF_RST=0;
}

void AD7606FSMCStart(void)
{
	ADIF_CV=0;
	ADIF_CV=0;
	ADIF_CV=0;	
	ADIF_CV=1;
}

void AD7606FSMCRead(s16* data)
{
	ADIF_CS=0;
	data[0]=AD7606_RESULT();
	data[1]=AD7606_RESULT();
	data[2]=AD7606_RESULT();
	data[3]=AD7606_RESULT();
	data[4]=AD7606_RESULT();
	data[5]=AD7606_RESULT();
	data[6]=AD7606_RESULT();
	data[7]=AD7606_RESULT();
	ADIF_CS=1;
}


