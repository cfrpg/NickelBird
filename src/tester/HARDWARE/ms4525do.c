#include "ms4525do.h"
#include "stdio.h"
#include "spi.h"
#include "math.h"
#include "delay.h"

float MS4525DOoffset;
float lastdp;
float averias;
int iascnt;
float iassum;
int iascnttgt;
void MS4525DOInit(void)
{
	GPIO_InitTypeDef gi;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);		
	
	//CS
	gi.GPIO_Pin=GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOG,&gi);
	MS4525DO_CS=1;
	SPI3Init();
	MS4525DOoffset=0;
	averias=0;
	iascnt=0;
	iassum=0;
	iascnttgt=10;
}

u8 ASRead(void)
{
//	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);	
//	SPI_I2S_SendData(SPI3,0xff);
//	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);	
//	u8 b=SPI_I2S_ReceiveData(SPI3);
//	
//	return b;
	return SPIRead(SPI3);
}

float MS4525DOReadAirspeed(float rho,float *pre)
{
	u8 b0,b1,b2,b3,f=0;
	u16 p,t;	
	float temp,dp,ias;
	MS4525DO_CS=0;
	SPIClearBuff(SPI3);
	b0=ASRead();	
	b1=ASRead();	
	b2=ASRead();
	t=b2;	
	b3=ASRead();
	MS4525DO_CS=1;
	p=b0&0x3F;
	p<<=8;
	p|=b1;
	t=(t<<3)|(b3>>5);
	temp=t*200/2047.0f-50;
	dp=(p-1638.3)/(0.8*16383/2)-1;
	if(rho>0.1)
		dp-=MS4525DOoffset;
	lastdp=dp;
	if(dp<0)
	{
		dp*=-1;
		f=1;
	}
	dp*=6894.757;
	if(rho==0)
		rho=1.125;
	ias=sqrt(2*dp/rho);
	if(f)
		ias=-ias;
	iassum+=ias;
	iascnt++;
	
	*pre=dp;
	//printf("%d %d %f %f %f %f\r\n",p,t,temp,rho,dp*6894.757,ias);
	return ias;	
}

void MS4525DOCali(u8 n)
{
	u8 i;
	float p;
	MS4525DOoffset=0;
	for(i=0;i<n;i++)
	{
		MS4525DOReadAirspeed(0,&p);
		MS4525DOoffset+=lastdp;
		delay_ms(10);
	}
	MS4525DOoffset/=n;
	printf("%f\r\n",MS4525DOoffset);
}

float MS4525DOReadAverageAirspeed()
{	
	averias=iassum/iascnt;
	iascnt=0;
	iassum=0;	
	return averias;
}
