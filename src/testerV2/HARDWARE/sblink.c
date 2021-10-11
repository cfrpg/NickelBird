#include "sblink.h"
#include "string.h"
#include "stdio.h"

u8 sendBuff[SENDBUFFSIZE];
u8 linkSeq;
u8 recBuff[2][256];
u8 recBuffLen[2];
u8 currBuff;
u8 recState;
u8 sblinkReady;
u8 recSum;
u8 pkgLen;

void LinkInit(void)
{
	GPIO_InitTypeDef gi;
	USART_InitTypeDef ui;	
	DMA_InitTypeDef di;
	NVIC_InitTypeDef ni;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	gi.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	gi.GPIO_Mode = GPIO_Mode_AF;
	gi.GPIO_Speed = GPIO_Speed_100MHz;
	gi.GPIO_OType = GPIO_OType_PP;
	gi.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&gi);

	ui.USART_BaudRate = 2000000;//921600;
	ui.USART_WordLength = USART_WordLength_8b;
	ui.USART_StopBits = USART_StopBits_1;
	ui.USART_Parity = USART_Parity_No;
	ui.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	ui.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &ui);
	USART_Cmd(USART1, ENABLE);
	
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	ni.NVIC_IRQChannel=USART1_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=3;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
	DMA_DeInit(DMA2_Stream7);
	
	while(DMA_GetCmdStatus(DMA2_Stream7)!=DISABLE);
	
	di.DMA_Channel = DMA_Channel_4;
	di.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
	di.DMA_Memory0BaseAddr = (u32)sendBuff;
	di.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	di.DMA_BufferSize = 0;
	di.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	di.DMA_MemoryInc = DMA_MemoryInc_Enable;
	di.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	di.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	di.DMA_Mode = DMA_Mode_Normal;
	di.DMA_Priority = DMA_Priority_VeryHigh;
	di.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	di.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	di.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	di.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &di);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);
	ni.NVIC_IRQChannel=DMA2_Stream7_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=0;
	ni.NVIC_IRQChannelSubPriority=3;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
		
	
	linkSeq=0;
	recState=0;
	currBuff=0;
	recBuffLen[0]=0;
	sblinkReady=0;
}

void LinkSendData(void* buff,u8 len)
{
	u8 i,sum=0;
	if(buff)
		memcpy(sendBuff,buff,len);
	sendBuff[2]=linkSeq;
	for(i=0;i<len;i++)
	{
		sum^=sendBuff[i];
	}
	sendBuff[len]=sum;
	
	linkSeq++;	
	//printf("%d\r\n",len);
	if(DMA_GetCurrDataCounter(DMA2_Stream7))
	{
		printf("dma %d\r\n",DMA_GetCurrDataCounter(DMA2_Stream7));
	}
	else
	{
		while(DMA_GetCurrDataCounter(DMA2_Stream7));		
		DMA_Cmd(DMA2_Stream7,DISABLE);	
		while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);	
		DMA_SetCurrDataCounter(DMA2_Stream7,len+1);
		DMA_Cmd(DMA2_Stream7,ENABLE);
		//PGout(13)=1;
	}
}

u32 LinkPackTime()
{
	RTCReadTime();
	return (time.hour<<22)|(time.min<<16)|(time.sec<<10)|time.ms;
}

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream7,DMA_IT_TCIF7)==SET)
	{
		if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_DMEIF7)!=RESET)
		{
			DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_DMEIF7);
		}
		
		DMA_ClearFlag(DMA2_Stream7,DMA_IT_TCIF7);
	}		
	//PGout(13)=0;	
}

void USART1_IRQHandler(void)
{
	
	u8 b;
	u8 curr=currBuff^1;
	printf("rx %x\r\n",b);
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_FLAG_ORE);
		printf("ore");
	}

	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		//USART_ClearFlag(USART1, USART_FLAG_RXNE);
		//USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		b=USART_ReceiveData(USART1);
		printf("rx %x\r\n",b);
		if(b==0xAC)
			sblinkReady=1;
//		//return;
//		if(recState!=0)
//		{
//			if(recState<9)
//				recSum^=b;
//			recBuff[curr][recBuffLen[curr]]=b;
//			recBuffLen[curr]++;
//		}
//		switch(recState)
//		{
//			case 0://get STX
//				if(b==0xFC)
//				{
//					recBuff[curr][0]=b;
//					recSum=0^b;
//					recBuffLen[curr]=1;
//					recState=1;
//				}
//			break;
//			case 1://get LEN
//				pkgLen=b;
//				recState=2;
//			break;
//			case 2://get SEQ
//				recState=3;
//			break;
//			case 3://get FUN
//				recState=4;
//			break;
//			case 4://get TIM1
//				recState=5;
//			break;
//			case 5://get TIM2
//				recState=6;
//			break;
//			case 6://get TIM3
//				recState=7;
//			break;
//			case 7://get TIM4
//				recState=8;
//			break;
//			case 8:
//				if(recBuffLen[curr]==pkgLen+8)
//				{
//					recState=9;
//				}
//			break;
//			case 9:
//				if(recSum==b)
//				{
//					currBuff^=1;
//					sblinkReady=1;
//				}
//				else
//				{
//					printf("Bad checksum %d %d\r\n",recSum,b);
//				}
//				recState=0;
//			break;
//		}
		
	}
}
