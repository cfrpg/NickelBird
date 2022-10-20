#include "sys.h"
#include "usart.h"	

#if 1
#pragma import(__use_no_semihosting)             
            
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
 
_sys_exit(int x) 
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

	ui.USART_BaudRate = baud;
	ui.USART_WordLength = USART_WordLength_8b;
	ui.USART_StopBits = USART_StopBits_1;
	ui.USART_Parity = USART_Parity_No;
	ui.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	ui.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &ui);
	
	USART_Cmd(USART2, ENABLE);
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
		
}
