#include "ad7606fsmc.h"
#include "sblink.h"
#include "nickelbird.h"
#include "sblink.h"

void AD7606FSMCInit(void)
{
	GPIO_InitTypeDef gi;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	
	//common GPIO	
	// Mode sense
	gi.GPIO_Pin=GPIO_Pin_3;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;		
	GPIO_Init(GPIOD,&gi);
	
	// Range sense
	gi.GPIO_Pin=GPIO_Pin_13;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;		
	GPIO_Init(GPIOC,&gi);
	
	
	
	//CS
	gi.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;		
	GPIO_Init(GPIOD,&gi);
	//RST
	gi.GPIO_Pin=GPIO_Pin_12;
	gi.GPIO_Mode=GPIO_Mode_OUT;	
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_DOWN;		
	GPIO_Init(GPIOD,&gi);
	//BUSY
	gi.GPIO_Pin=GPIO_Pin_13;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&gi);
	
	ADIF_CS1=1;
	ADIF_CS2=1;
	
	//FSMC GPIO
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//D3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);//NOE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);//NWE
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);//CS
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
	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);//D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);//D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);//D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);//D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);//D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);//D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);//D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);//D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);//D12

	gi.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	GPIO_Init(GPIOE, &gi);

	//FSMC Config
	FSMC_NORSRAMInitTypeDef  fi;
	FSMC_NORSRAMTimingInitTypeDef  timing;
	
	timing.FSMC_AddressSetupTime = 0;
	timing.FSMC_AddressHoldTime = 0;
	timing.FSMC_DataSetupTime = 4;
	timing.FSMC_BusTurnAroundDuration = 0;
	timing.FSMC_CLKDivision = 0;
	timing.FSMC_DataLatency = 0;
	timing.FSMC_AccessMode = FSMC_AccessMode_A;
	
	fi.FSMC_Bank = FSMC_Bank1_NORSRAM1;
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
	
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
	
	AD7606FSMCReset();	
	
	//PD13 EXTI
	EXTI_InitTypeDef ei;
	NVIC_InitTypeDef ni;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource13);
	
	ei.EXTI_Line=EXTI_Line13;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI15_10_IRQn;
	ni.NVIC_IRQChannelCmd=ENABLE;
	ni.NVIC_IRQChannelPreemptionPriority=0;
	ni.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&ni);
	
	sys.sensors.Config&=0b11000000;
	if(ADIF_RNG_SENS==1)
	{
		sys.sensors.Config|=RANGE_10V0BP;
		sys.sensors.Config|=RANGE_10V0BP<<3;
	}
	else
	{
		sys.sensors.Config|=RANGE_5V0BP;
		sys.sensors.Config|=RANGE_5V0BP<<3;
	}
}
//void AD7606FSMCSetInternalClk(void)
//{
//	//set gpio to af
//	GPIO_PinAFConfig(ADIF_CV_GPIO,ADIF_CV_Pin,GPIO_AF_TIM3);
//	ADIF_CV_GPIO->MODER  &= ~(GPIO_MODER_MODER0 << (ADIF_CV_Pin * 2));
//    ADIF_CV_GPIO->MODER |= (((uint32_t)0x02) << (ADIF_CV_Pin * 2));
//	//start tim3
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	TIM_Cmd(TIM3, ENABLE);
//}

//void AD7606FSMCSetExternalClk(void)
//{
//	//stop tim3	
//	TIM_Cmd(TIM3, DISABLE);
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	//set gpio to out
//	GPIO_PinAFConfig(ADIF_CV_GPIO,ADIF_CV_Pin,(u8)0x00);
//	ADIF_CV_GPIO->MODER  &= ~(GPIO_MODER_MODER0 << (ADIF_CV_Pin * 2));
//    ADIF_CV_GPIO->MODER |= (((uint32_t)0x01) << (ADIF_CV_Pin * 2));
//	ADIF_CV=1;
//}

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
	ADIF_CS1=0;
	data[0]=AD7606_RESULT();
	data[1]=AD7606_RESULT();
	data[2]=AD7606_RESULT();
	data[3]=AD7606_RESULT();
	data[4]=AD7606_RESULT();
	data[5]=AD7606_RESULT();
	data[6]=AD7606_RESULT();
	data[7]=AD7606_RESULT();
	ADIF_CS1=1;
	ADIF_CS2=0;
	data[8]=AD7606_RESULT();
	data[9]=AD7606_RESULT();
	data[10]=AD7606_RESULT();
	data[11]=AD7606_RESULT();
	data[12]=AD7606_RESULT();
	data[13]=AD7606_RESULT();
	data[14]=AD7606_RESULT();
	data[15]=AD7606_RESULT();
	ADIF_CS2=1;	
}


