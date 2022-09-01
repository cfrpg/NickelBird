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
#include "ads1256.h"
#include "eo.h"
#include "pwm.h"
#include "sblink.h"
#include "rtc.h"
#include "adc.h"
#include "ad7606.h"
#include "ad7606fsmc.h"
#include "i2c.h"
#include "ms4525do.h"
#include "spi.h"
#include "bmp280.h"
#include "sdp3x.h"
#include "extin.h"
#include "sensors.h"
#include "dac.h"
#include "math.h"

#define TEST0 PGout(9)
#define TEST1 PGout(11)
#define TEST2 PGout(13)
#define TEST3 PGout(15)

void initTestGPIO(void);

u16 tick[3]={0,0,0};
u16 cpucnt=0;
u8 lastKey=0;
u8 currKey=0;
s32 lastWheel=0;
s32 currWheel=0;

s8 currpage;
systemState sys;

u8 package[256];

s16 adraw[8];

u32 dacclk;

int main(void)
{
	s16 i;
	u8 key;
	delay_init(168);
	uart_init(115200);
	
	MainClockInit();
	SPIInit();
	LEDInit();
	//initTestGPIO();
	delay_ms(100);
	LinkInit();	
	ADCInit();
	AD7606FSMCInit();		
	BMPInit();
	MS4525DOInit();
	I2CInit();
	ExtinInit();
	
	DACInit();
	dacclk=0;
	
	while(tick[0]<200);
	LEDSet(1);
	
	lastKey=0xFF;		
	
	sys.sensors.header.stx=LINKSTX;
	sys.sensors.header.len=DataLen[SENSOR_DATA];
	sys.sensors.header.fun=SENSOR_DATA;	
	sys.sensors.header.time=0;
	printf("NickelBird\r\n");
	delay_ms(100);
	MS4525DOCali(200);
	sys.temperature=BMPReadTemperature();
	sys.pressure=BMPReadPressure();
	sys.rho=BMPReadAverageRho(200);
	Sdp3xInit(SDP3X_ADDR1);
	Sdp3xSetMode(SDP3X_ADDR1,0x3615);
	Sdp3xCali(SDP3X_ADDR1,200);
	SeneorsInit();
	delay_ms(100);
	while(1)
	{	
		if(tick[0]>=500)
		{
			tick[0]=0;
			LEDFlip();
		}
		if(tick[1]>=2)
		{
			tick[1]=0;
			if(sblinkReady)
			{
				//memcpy(package,recBuff[currBuff],recBuffLen[currBuff]);
				//sblinkReady=0;
				//printf("Rec Fun=%d,Len=%d\r\n",package[1],package[2]);
				LEDFlip();
				delay_ms(200);
				LEDFlip();
				delay_ms(200);
				LEDFlip();
				
				MS4525DOCali(200);
				sys.temperature=BMPReadTemperature();
				sys.pressure=BMPReadPressure();
				sys.rho=BMPReadAverageRho(200);
				LEDFlip();
				delay_ms(200);
				LEDFlip();
				delay_ms(200);
				LEDFlip();
				sblinkReady=0;
			}
			SensorsFastUpdate();
		}
		if(tick[2]>=50)
		{
			tick[2]=0;		
			SensorsSlowUpdate();			
		}
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
		dacclk++;
		DACSet(1,(sinf(2*3.14159f*4*dacclk/1000)+1)/2);
		DACSet(2,(sinf(2*3.14159f*3*dacclk/1000)+1)/2);
//		AD7606FSMCRead(sys.sensors.ADCData);
//		AD7606FSMCStart();
//		LinkSendData(&sys.sensors,sizeof(SensorDataPackage));
//		if(RTCcnt%4==0)
//			UpdateSensors();
		
	}
}

void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		//TEST1=1;
		AD7606FSMCRead(sys.sensors.ADCData);		
		
		LinkSendData(&sys.sensors,sizeof(SensorDataPackage));
		if(sys.intEnabled[0]>=0)
			sys.sensors.SensorData[sys.intEnabled[0]]=0;
		if(sys.intEnabled[1]>=0)
			sys.sensors.SensorData[sys.intEnabled[1]]=0;

		EXTI_ClearITPendingBit(EXTI_Line2);
		//TEST1=0;
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//TEST0=1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		ADCReadVol(sys.sensors.ADCData+8);
		SensorsIntUpdate();
		sys.sensors.header.time++;
		//TEST0=0;
	}
}

void initTestGPIO(void)
{
	GPIO_InitTypeDef gi;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_15;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOG,&gi);
	
	TEST0=0;
	TEST1=0;
	TEST2=0;
	TEST3=0;	
}
