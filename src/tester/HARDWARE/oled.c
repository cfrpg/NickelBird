#include "oled.h"
#include "delay.h"
#include "string.h"
#include "font.h"
#include "stdio.h"

u8 oledBuff[16][22];
u16 oledRevFlag[21];

void OledInit(void)
{
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	//Init SPI GPIO
	gi.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_15;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&gi);
	//Init common GPIO
	gi.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_14;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&gi);
	gi.GPIO_Pin=GPIO_Pin_8;
	GPIO_Init(GPIOD,&gi);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);
	//Init SPI2
	si.SPI_Direction=SPI_Direction_1Line_Tx;
	si.SPI_Mode=SPI_Mode_Master;
	si.SPI_DataSize=SPI_DataSize_8b;
	si.SPI_CPOL=SPI_CPOL_Low;
	si.SPI_CPHA=SPI_CPHA_1Edge;
	si.SPI_NSS=SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	si.SPI_FirstBit=SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial=7;
	SPI_Init(SPI2,&si);
	SPI_Cmd(SPI2,ENABLE);
	memset(oledBuff,0,sizeof(oledBuff));	
	memset(oledRevFlag,0,sizeof(oledRevFlag));
	OledStartUp();
}

void OledSendByte(u8 b)
{
	OLED_CS=0;
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI2,b);	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	//delay_us(2);
	//OLED_CS=1;
}

void OledStartUp(void)
{
	OLED_RST=1;
	delay_ms(100);
	OLED_RST=0;
	delay_ms(100);
	OLED_RST=1;
	delay_ms(100);
	OLED_DC=OLED_CMD;
	OledSendByte(0xae);//Set display off
	OledSendByte(0xa0);//Set re-map
	OledSendByte(0x55);
	OledSendByte(0xa1);//Set display start line
	OledSendByte(0x00);
	OledSendByte(0xa2);//Set display offset
	OledSendByte(0x00);
	OledSendByte(0xa4);//Normal Display
	OledSendByte(0xa8);//Set multiplex ratio
	OledSendByte(0x7f);
	OledSendByte(0xab);//Function Selection A
	OledSendByte(0x01);//Enable internal VDD regulator
	OledSendByte(0x81);//Set contrast
	OledSendByte(0x77);
	OledSendByte(0xb1);//Set Phase Length
	OledSendByte(0x31);
	OledSendByte(0xb3);//Set Front Clock Divider /Oscillator Frequency
	OledSendByte(0xb1);
	OledSendByte(0xb4); //For brightness enhancement
	OledSendByte(0xb5);
	OledSendByte(0x03);
	OledSendByte(0xb6);//Set Second pre-charge Period
	OledSendByte(0x0d);
	OledSendByte(0xbc);//Set Pre-charge voltage
	OledSendByte(0x07);
	OledSendByte(0xbe);//Set VCOMH
	OledSendByte(0x07);
	OledSendByte(0xd5);//Function Selection B
	OledSendByte(0x02);//Enable second pre-charge
	//OledSendByte(0xb9);
	OledSendByte(0xaf);//Display on	
	OledSendByte(0xb5);
	OledSendByte(0x03);
}

void OledDispOn(void)
{
	OLED_DC=OLED_CMD;
	OledSendByte(0x8D);//--set Charge Pump enable/disable
	OledSendByte(0x14);
	OledSendByte(0xAF);//display ON 
}

void OledDispOff(void)
{
	OLED_DC=OLED_CMD;
	OledSendByte(0x8D);//--set Charge Pump enable/disable
	OledSendByte(0x10);
	OledSendByte(0xAE);//display ON 
}

void OledSetPos(u8 x,u8 y)
{
	OLED_DC=OLED_CMD;
	OledSendByte(0xB0+y);
	OledSendByte(((x&0xF0)>>4)|0x10);
	OledSendByte((x&0x0F)|0x01);
}

void oledSelectRect(u8 r1,u8 r2,u8 c1,u8 c2)
{
	OLED_DC=OLED_CMD;	
	OledSendByte(0x15);
	OledSendByte(c1);
	OledSendByte(c2);
	OledSendByte(0x75);
	OledSendByte(r1);
	OledSendByte(r2);
}

void OledClearBuff(void)
{
	memset(oledBuff,0,sizeof(oledBuff));	
	memset(oledRevFlag,0,sizeof(oledRevFlag));
}

void OledClear(u8 c)
{
	u8 i,j;
	c=c|(c<<4);
	oledSelectRect(0,127,0,63);
	//delay_us(100);
	OLED_DC=OLED_DATA;
	//delay_us(100);
	for(i=0;i<128;i++)
	{
		for(j=0;j<64;j++)
		{
			OledSendByte(c);
		}
	}
	//delay_us(100);
}

void OledRefresh(void)
{
	u8 i,j,k,l;
	u16 m;
	u8 c;
	oledSelectRect(0,127,0,63);
	OLED_DC=OLED_DATA;
	for(j=0;j<21;j++)
	{
		for(k=0;k<6;k+=2)
		{
			for(i=0;i<16;i++)
			{
				m=1<<i;
				for(l=0;l<8;l++)
				{
					c=0;
					if(F6x8[oledBuff[i][j]][k]&(1<<l))
						c=0xF0;
					if(F6x8[oledBuff[i][j]][k+1]&(1<<l))
						c|=0x0F;
					if(oledRevFlag[j]&m)
						OledSendByte(c^0xFF);
					else
						OledSendByte(c);
				}
			}
		}		
	}	
}

void OledSetChar(u8 x,u8 y,s8 c,u8 f)
{
	oledBuff[y][x]=c-' ';
	if(f)
		oledRevFlag[x]|=(0x0001<<y);
	else
		oledRevFlag[x]&=((0x0001<<y)^0xFFFF);	
}

u8 OledSetString(u8 x,u8 y,s8 c[],u8 f)
{
	u8 i;
	
	for(i=0;c[i]!=0;i++)
	{
		oledBuff[y][x+i]=c[i]-' ';
		if(f)
			oledRevFlag[x+i]|=(0x0001<<y);
		else
			oledRevFlag[x+i]&=((0x0001<<y)^0xFFFF);
	}
	return i;
}

void OledSetRev(u8 x,u8 y,s8 len,u8 f)
{	
	while(len--)
	{
		if(f)
			oledRevFlag[x+len]|=(0x0001<<y);
		else
			oledRevFlag[x+len]&=((0x0001<<y)^0xFFFF);
	}
}

void OledRefreshPart(u8 x,u8 y,s8 len)
{	
	s8 j,k,l;
	u16 m=1<<y;
	u8 c;
	oledSelectRect(y<<3,(y<<3)+7,x*3,(x+len)*3-1);
	OLED_DC=OLED_DATA;
	for(j=0;j<len;j++)
	{
		for(k=0;k<6;k+=2)
		{							
			for(l=0;l<8;l++)
			{
				c=0;
				if(F6x8[oledBuff[y][j+x]][k]&(1<<l))
					c=0xF0;
				if(F6x8[oledBuff[y][j+x]][k+1]&(1<<l))
					c|=0x0F;
				if(oledRevFlag[j+x]&m)
					OledSendByte(c^0xFF);
				else
					OledSendByte(c);
			}			
		}		
	}	
}

void OledDispChar(u8 x,u8 y,s8 c,u8 f)
{
	OledSetChar(x,y,c,f);
	OledRefreshPart(x,y,1);	
}

void OledDispString(u8 x,u8 y,s8 c[],u8 f)
{	
	u8 t=OledSetString(x,y,c,f);
	OledRefreshPart(x,y,t);
}

void OledDispInt(u8 x,u8 y,s32 v,u8 ml,u8 f)
{
	s8 buf[12],p=0,len=0;
	s32 t;
	buf[ml]=0;
	if(v<0)
	{
		v=-v;
		p=1;
		buf[0]='-';
		len++;
	}
	t=v;
	
	do{
		t/=10;
		len++;
	}while(t>0);		
	buf[len]=0;
	while(ml>len)
	{
		buf[ml-1]=' ';
		ml--;
	}
	len--;
	while(len>=p)
	{
		buf[len]=v%10+'0';
		v/=10;
		len--;
	}
	OledDispString(x,y,buf,f);	
}

void OledDispDouble(u8 x,u8 y,float v,s8 len,s8 pre,u8 f)
{
	
}

void OledDispFixed(u8 x,u8 y,s32 v,s8 pre,u8 ml,u8 f)
{
	s8 buf[14],p=0,len=0;
	s32 t;
	buf[ml]=0;
	if(v<0)
	{
		v=-v;
		p=1;
		buf[0]='-';		
	}
	t=v;	
	do{
		t/=10;
		len++;
	}while(t>0);	
	if(len<=pre)
		len=pre+1;
	if(p)
		len++;
	pre=len-pre;
	buf[len+1]=0;	
	while(ml>len+1)
	{
		buf[ml-1]=' ';
		ml--;
	}		
	while(len>=p)
	{
		if(len==pre)
			buf[len]='.';
		else
		{
			buf[len]=v%10+'0';
			v/=10;
		}
		len--;
	}
	OledDispString(x,y,buf,f);	
}
