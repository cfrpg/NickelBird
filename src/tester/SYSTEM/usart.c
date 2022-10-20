#include "sys.h"
#include "usart.h"	

#if 1
#pragma import(__use_no_semihosting)        

u8 USART_RX_BUF[USART_REC_LEN]; 
u16 USART_RX_STA=0;
            
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
 
void _sys_exit(int x) 
{ 
	x = x; 
} 
 
int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0);
	USART2->DR = (u8) ch;      
	return ch;
}
#endif

void uart_init(u32 baud)
{   
	GPIO_InitTypeDef gi;
	USART_InitTypeDef ui;	
	
	NVIC_InitTypeDef ni;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	gi.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gi.GPIO_Mode = GPIO_Mode_AF;
	gi.GPIO_Speed = GPIO_Speed_100MHz;
	gi.GPIO_OType = GPIO_OType_PP;
	gi.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&gi);
	
	ni.NVIC_IRQChannel=USART2_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=3;
	ni.NVIC_IRQChannelSubPriority=3;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);

	ui.USART_BaudRate = baud;
	ui.USART_WordLength = USART_WordLength_8b;
	ui.USART_StopBits = USART_StopBits_1;
	ui.USART_Parity = USART_Parity_No;
	ui.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	ui.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &ui);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART2, ENABLE);
	
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
		
}

void USART2_IRQHandler(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		
		if((USART_RX_STA&0x8000)==0)
		{
			if(USART_RX_STA&0x4000)
			{
				if(Res=='/')
				{
					USART_RX_STA|=0x8000;
				}
				else 
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if((USART_RX_STA&0X3FFF)>(USART_REC_LEN-1))USART_RX_STA=0;
				}
			}
			else
			{
				if(Res=='*')USART_RX_STA=0x4000;				
			}
		}
		//printf("rx %c %d\r\n",Res,USART_RX_STA>>13);		
    } 
} 
