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
#include "adc.h"
#include "ad7606fsmc.h"
#include "i2c.h"
#include "spi.h"
#include "extin.h"
#include "sensors.h"
#include "mb85rs.h"
#include "logo.h"

#define TEST0 PEout(0)
#define TEST1 PEout(1)


void initTestGPIO(void);
void AnalyzePkg(void);

u16 tick[3]={0,0,0};
u16 cpucnt=0;
u8 lastKey=0;
u8 currKey=0;
u8 lastWheelPush=0;
u8 currWheelPush=0;
s32 lastWheel=0;
s32 currWheel=0;
u8 keyPress=0;
u8 wheelPress=0;

s8 currpage;
systemState sys;

u8 package[256];

s16 adraw[8];



int main(void)
{
	s16 i;
	s16 tmp[2];
	u8 key;
	delay_init(168);
	uart_init(115200);
	
	OledInit();
	MainClockInit();
	LEDInit();
	OledClear(0x00);

	OledDispBitmap(0,0,128,128,gImage_logo);
	FRAMInit();
	ParamRead();
	if(params.headFlag!=0xCFCF||params.tailFlag!=0xFCFC)
	{
		OledDispString(0,0,"Reset Parameters.",0);
		ParamReset();
		ParamWrite();
	}
	PWMInit();
	
//	initTestGPIO();
	PagesInit();
	LinkInit();	
	ADCInit();
	AD7606FSMCInit();
	I2CInit();
	ExtinInit();

	KeyInit();

	printf("NickelBird\r\n");
	
	SeneorsInit();
	
	delay_ms(500);
	delay_ms(500);
	while(1)
	{	
		if(tick[0]>=500)
		{
			tick[0]=0;
			LEDFlip();
			if(USART_RX_STA&0x8000)
			{				
				AnalyzePkg();
				USART_RX_STA=0;
			}
		}
		if(tick[1]>=2)
		{
			tick[1]=0;			
			SensorsFastUpdate();
		}
		if(tick[2]>=200)
		{
			tick[2]=0;
			SensorsSlowUpdate();
			PagesUpdate();
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
		ParamReset();
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
		if(cpucnt>10000)
			cpucnt=0;
		RTCcnt++;
		if(RTCcnt>999)
			RTCcnt=999;		
	}
}

void EXTI15_10_IRQHandler(void)
{
	//printf("int\r\n");
	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		AD7606FSMCRead(sys.sensors.ADCData);
		ADCReadVol(sys.sensors.ADCData+8);
		LinkSendData(&sys.sensors,sizeof(SensorDataPackage));
		SensorsIntUpdate(); 
		EXTI_ClearITPendingBit(EXTI_Line13);
		//TEST1=0;
	}
	if (EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		WheelUpdate();
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//TEST1=1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		ADCStartConv();
		sys.sensors.header.time++;
		//TEST0=0;
	}
}

void initTestGPIO(void)
{
	GPIO_InitTypeDef gi;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOE,&gi);
	
	TEST0=0;
	TEST1=0;
}
