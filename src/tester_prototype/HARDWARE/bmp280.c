#include "bmp280.h"
#include "spi.h"
#include "delay.h"
#include "stdio.h"

#define BMP280_W 0x7F

#define BMP280_ID 0xD0
#define BMP280_RESET 0xE0
#define BMP280_STATUS 0xF3
#define BMP280_CTRL 0xF4
#define BMP280_CONFIG 0xF5
#define BMP280_PRESS 0xF7
#define BMP280_TEMP 0xFA

u16 dig_T1;
s16 dig_T2;
s16 dig_T3;
u16 dig_P1;
s16 dig_P2;
s16 dig_P3;
s16 dig_P4;
s16 dig_P5;
s16 dig_P6;
s16 dig_P7;
s16 dig_P8;
s16 dig_P9;

s32 t_fine;

void BMPInit(void)
{
	GPIO_InitTypeDef gi;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);		
	
	//CS
	gi.GPIO_Pin=GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;	
	GPIO_Init(GPIOG,&gi);
	
	//BMP280_CS=1;
	BMP280_CS=0;
	delay_us(1);
	BMP280_CS=1;
	
	SPI3Init();
	BMPReadCali();
	BMPConfigOS(BMP_OS_1,BMP_OS_1,BMP_Mode_Normal);
	//BMPConfigRate(BMP_Tsb_Halfms,0);
}

void bmpWrite(u8 b)
{
	//SPIWrite(SPI3,b);
	SPIReadWrite(SPI3,b);
}

u8 bmpRead(void)
{
	return SPIRead(SPI3);
}

void BMPReadCali(void)
{
	u16 lsb,msb;
	BMP280_CS=0;
	
	bmpWrite(0x88);
	//SPIClearBuff(SPI3);
	lsb=bmpRead();
	msb=bmpRead();
	printf("%x %x\r\n",lsb,msb);
	dig_T1=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_T2=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_T3=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P1=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P2=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P3=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P4=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P5=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P6=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P7=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P8=(msb<<8)|lsb;
	lsb=bmpRead();
	msb=bmpRead();
	dig_P9=(msb<<8)|lsb;
	BMP280_CS=1;
	//printf("Cali %d %d %d\r\n",dig_T1,dig_T2,dig_T3);
}

void BMPConfigOS(u8 t,u8 p,u8 m)
{
	u8 b=0;
	b|=(m&0x03);
	b|=(p&0x07)<<2;
	b|=(t&0x07)<<5;
	BMP280_CS=0;	
	//delay_us(1);
	bmpWrite(BMP280_CTRL&BMP280_W);
	bmpWrite(b);
	BMP280_CS=1;
	
}

void BMPConfigRate(u8 t,u8 iir)
{
	u8 b=0;
	b|=(iir&0x07)<<2;
	b|=(t&0x07)<<5;
	BMP280_CS=0;	
	delay_us(1);
	bmpWrite(BMP280_CONFIG&BMP280_W);
	bmpWrite(b);
	BMP280_CS=1;
	
}

float BMPReadPressure(void)
{
	s32 adc_P;
	s32 var1,var2;
	u32 p;
	u8 b;
	BMP280_CS=0;
	//delay_us(1);
	bmpWrite(BMP280_PRESS);
	//SPIClearBuff(SPI3);
	b=bmpRead();
	adc_P|=b;
	adc_P<<=8;
	b=bmpRead();
	adc_P|=b;
	adc_P<<=4;
	b=bmpRead();
	adc_P|=(b>>4);
	BMP280_CS=1;
	var1 = (((s32)t_fine)>>1) - (s32)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((s32)dig_P6);
	var2 = var2 + ((var1*((s32)dig_P5))<<1);
	var2 = (var2>>2)+(((s32)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((s32)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((s32)dig_P1))>>15);
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
		p = (((s32)(((s32)1048576)-adc_P)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((s32)var1);
	}
	else
	{
		p = (p / (s32)var1) * 2;
	}
	var1 = (((s32)dig_P9) * ((s32)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((s32)(p>>2)) * ((s32)dig_P8))>>13;
	p = (s32)((s32)p + ((var1 + var2 + dig_P7) >> 4));
	//printf("p %d %d\r\n",adc_P,p);
	return (float)p;
}

float BMPReadTemperature(void)
{
	s32 adc_T=0;
	s32 var1,var2,T;
	u8 b0,b1,b2;
	BMP280_CS=0;
	//delay_us(1);
	bmpWrite(BMP280_TEMP);
	//SPIClearBuff(SPI3);
	b0=bmpRead();
	adc_T|=b0;
	adc_T<<=8;
	b1=bmpRead();
	adc_T|=b1;
	adc_T<<=4;
	b2=bmpRead();
	adc_T|=(b2>>4);
	BMP280_CS=1;
	var1 = ((((adc_T>>3) - ((s32)dig_T1<<1))) * ((s32)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((s32)dig_T1)) * ((adc_T>>4) - ((s32)dig_T1))) >> 12) *((s32)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	//printf("t %d %f %x %x %x\r\n",adc_T,T/100.0,b0,b1,b2);
	return ((float)T)/100;
}

float BMPReadAverageRho(u8 n)
{
	u8 i;
	float t,p,r;
	r=0;
	for(i=0;i<n;i++)
	{
		t=BMPReadTemperature();
		p=BMPReadPressure();
		r+=1.293*273/(273+t)*p/101325;
		delay_ms(5);
	}
	r/=n;
	return r;
}
