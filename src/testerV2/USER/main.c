#include "stm32f4xx.h"
#include "string.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "oled.h"
#include "timer.h"
#include "keyboard.h"
#include "parameter.h"
#include "pages.h"
#include "pwm.h"
#include "sblink.h"
#include "ad7606fsmc.h"
#include "extin.h"
#include "mb85rs.h"
#include "logo.h"
#include "sampling.h"

#include "dac.h"

#define TEST0 PBout(10)
#define TEST1 PBout(11)


void initTestGPIO(void);
void AnalyzePkg(void);

u32 tick[3]={0,0,0};
u16 cpucnt=0;


s8 currpage;
systemState sys;

u8 package[256];

s16 adraw[8];

int main(void)
{
	s16 i,j;
//	s16 tmp[2];
	u16 time;
	
	// Initialize system function
	delay_init(168);
	uart_init(921600);
	MainClockInit();
	
	// Initialize hardware
	OledInit();	
	OledClear(0x00);	
	LEDInit();
	KeyInit();
	FRAMInit();	
	
	// Show logo
	printf("NickelBird\r\n");	
	OledDrawBitmap(0,0,128,128,gImage_logo);
	OledRefresh();
	
	// Load parameters
	u8 t=ParamRead();
	if(t)
	{
		if(t==255)
		{
			OledDrawString(0,0,"Reset all parameters.",0);
		}
		else
		{
			OledDrawString(0,0,"Reset     parameters.",0);
			OledDrawInt(6,0,t,3,0);
		}
		delay_ms(500);
		delay_ms(500);
	}
	
	//Initialize DAQ peripherals
	AD7606FSMCInit();
	DACInit();
	PWMInit();
	PWMSetAux(1500,1500);
	//	ExtinInit();
	
	// Initialize sampling control
	SamplingInit();
	//SamplingSetClockSource(EXTERNAL);
	LinkInit();	
	
//	u8 buff[100];
//	memset(buff,0x55,100);
//	while(1)
//	{
//		LinkSendRawData(buff,100);
//		while(DMA_GetCurrDataCounter(DMA1_Stream6)!=0);
//		delay_us(100);
//	}
	
	//Initialize UI
	PagesInit();
//	initTestGPIO();	
	
	// System start
	LEDSetPattern(LED_OFF,LED_OFF,LED_1Hz);
	delay_ms(500);
	delay_ms(500);
	
	//printf("%d\t%d\n",sizeof(PackageHeader),sizeof(SensorDataPackage));

	while(1)
	{	
		if(tick[0]>=201)
		{			
			tick[0]=0;
			LEDUpdate();
			if(USART_RX_STA&0x8000)
			{				
				AnalyzePkg();
				USART_RX_STA=0;
			}
			
		}
		if(tick[1]>=2)
		{
			time=tick[1];
			tick[1]=0;	
			SamplingFastUpdate(time);
		}
		if(tick[2]>=101)
		{
			time=tick[2];
			tick[2]=0;
			SamplingSlowUpdate(time);
			PagesUpdate();
			OledRefresh();
		}
	}
}

void AnalyzePkg(void)
{
	u8 len=USART_RX_STA&0x3FFF;
	u8 t,i;
	s32 v;
	
	if(USART_RX_BUF[0]=='s'&&USART_RX_BUF[1]=='e'&&USART_RX_BUF[2]=='t')
	{
		printf("SET CMD\r\n");
		if(len!=14)
		{
			printf("Invalid length:%d.\r\n",len);		
			return;
		}
		t=(USART_RX_BUF[3]-'0')*10+(USART_RX_BUF[4]-'0');
		printf("ID:%d\r\n",t);
		v=0;
		for(i=0;i<8;i++)
		{
			v*=10;
			v+=USART_RX_BUF[6+i]-'0';
		}
		if(USART_RX_BUF[5]=='-')
			v*=-1;
		printf("Value:%d\r\n",v);
		if(ParamSet(t,v)==0)
		{
			printf("Complete.\r\n");
			
		}
		else
		{
			printf("Failed.\r\n");
			
		}
		return;
	}
	if(USART_RX_BUF[0]=='s'&&USART_RX_BUF[1]=='h'&&USART_RX_BUF[2]=='o'&&USART_RX_BUF[3]=='w')
	{
		printf("SHOW CMD\r\n");
		ParamShow();
	}
	if(USART_RX_BUF[0]=='r'&&USART_RX_BUF[1]=='s'&&USART_RX_BUF[2]=='t'&&USART_RX_BUF[3]=='p')
	{
		printf("RSTP CMD\r\n");
		ParamReset(0);
		ParamWrite();
		printf("Param reset complete.\r\n");
		ParamShow();
	}
}

void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		u8 n=3;
		while(n--)
		{
			tick[n]++;
		}
		cpucnt++;
		if(cpucnt>=(10000))
			cpucnt=0;
		
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		SamplingIntUpdate(); 
	}
	if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		WheelUpdate();
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
}



void initTestGPIO(void)
{
	GPIO_InitTypeDef gi;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&gi);
	
	TEST0=0;
	TEST1=0;
}
