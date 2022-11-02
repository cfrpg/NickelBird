#include "oled.h"
#include "delay.h"
#include "string.h"
#include "font.h"
#include "stdio.h"

#define OLED_BUFF_SIZE 8192

u8 oledBuff[64][128];

void OledInit(void)
{
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	//Init SPI GPIO
	gi.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&gi);
	//Init common GPIO
	gi.GPIO_Pin=GPIO_Pin_6;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&gi);
	gi.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init(GPIOC,&gi);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);
	//Init SPI1
	si.SPI_Direction=SPI_Direction_1Line_Tx;
	si.SPI_Mode=SPI_Mode_Master;
	si.SPI_DataSize=SPI_DataSize_8b;
	si.SPI_CPOL=SPI_CPOL_Low;
	si.SPI_CPHA=SPI_CPHA_1Edge;
	si.SPI_NSS=SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
	si.SPI_FirstBit=SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial=7;
	SPI_Init(SPI1,&si);
	SPI_Cmd(SPI1,ENABLE);
	memset(oledBuff,0,sizeof(oledBuff));	
	OledStartUp();
	
	OledClearBuff();
	
	// DMA config
	DMA_InitTypeDef di;
	NVIC_InitTypeDef ni;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	
	DMA_DeInit(DMA2_Stream5);	
	while(DMA_GetCmdStatus(DMA2_Stream5)!=DISABLE);
	
	di.DMA_Channel = DMA_Channel_3;
	di.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;
	di.DMA_Memory0BaseAddr = (u32)oledBuff;
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
	DMA_Init(DMA2_Stream5, &di);
	
	SPI_DMACmd(SPI1,SPI_DMAReq_Tx,ENABLE);
	
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
	
	ni.NVIC_IRQChannel=DMA2_Stream5_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=3;
	ni.NVIC_IRQChannelSubPriority=3;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);	
}

void DMA2_Stream5_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream5,DMA_IT_TCIF5)==SET)
	{
		if(DMA_GetFlagStatus(DMA2_Stream5,DMA_FLAG_DMEIF5)!=RESET)
		{
			DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_DMEIF5);
		}		
		DMA_ClearFlag(DMA2_Stream5,DMA_IT_TCIF5);
	}		
	//PGout(13)=0;	
}



void OledSendByte(u8 b)
{
	OLED_CS=0;
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,b);	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	//delay_us(1);
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
	OledSendByte(0x46);
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
	OledSendByte(0xf4);
	OledSendByte(0xb3);//Set Front Clock Divider /Oscillator Frequency
	OledSendByte(0xf0);//b1
	//OledSendByte(0xb4); //For brightness enhancement
	OledSendByte(0xb5);
	OledSendByte(0x03);
	OledSendByte(0xb6);//Set Second pre-charge Period
	OledSendByte(0x0d);
	OledSendByte(0xbc);//Set Pre-charge voltage
	OledSendByte(0x08);
	OledSendByte(0xbe);//Set VCOMH
	OledSendByte(0x07);
	OledSendByte(0xd5);//Function Selection B
	OledSendByte(0x02);//Enable second pre-charge
	//OledSendByte(0xb9);
	OledSendByte(0xaf);//Display on	
	OledSendByte(0xb5);
	OledSendByte(0x03);
	OledDispOn();
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
	memset(oledBuff,0xFF,sizeof(oledBuff));	
}

void OledClear(u8 c)
{
	u8 i,j;	
	c=c|(c<<4);
	oledSelectRect(0,127,0,63);
	OLED_DC=OLED_DATA;	
	for(i=0;i<128;i++)
	{
		for(j=0;j<64;j++)
		{
			OledSendByte(c);
		}
	}
}

void OledRefresh(void)
{
	u8 i,j;
	oledSelectRect(0,127,0,63);
	OLED_DC=OLED_DATA;
	for(i=0;i<64;i++)
	{
		for(j=0;j<128;j++)
		{
			OledSendByte(oledBuff[i][j]);
		}
	}
}

void OledStartRefresh(void)
{	
	while(DMA_GetCurrDataCounter(DMA2_Stream5));		
	DMA_Cmd(DMA2_Stream5,DISABLE);	
	while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);		
	DMA_SetCurrDataCounter(DMA2_Stream5,OLED_BUFF_SIZE);
	SPI1->DR;
	OLED_DC=OLED_DATA;
	OLED_CS=0;
	DMA_ClearFlag(DMA2_Stream5,DMA_IT_TCIF5);	
	DMA_Cmd(DMA2_Stream5,ENABLE);
	
}

void OledDrawChar(u8 x,u8 y,s8 c,u8 f)
{
	u8 i,j,m=0;
	u8 t1,t2;
	x*=3;
	y<<=3;
	c-=' ';
	if(f)
		m=0xFF;
	for(i=0;i<3;i++)
	{
		if(f)
			memcpy(&(oledBuff[x+i][y]),&(F6x8gf[c][i<<3]),8);
		else
			memcpy(&(oledBuff[x+i][y]),&(F6x8g[c][i<<3]),8);
	}
	
}

u8 OledDrawString(u8 x,u8 y,s8 c[],u8 f)
{
	u8 i;	
	for(i=0;c[i]!=0;i++)
	{
		OledDrawChar(x+i,y,c[i],f);		
	}
	return i;
}

void OledSetStringRev(u8 x,u8 y,s8 len,u8 f)
{	
	u8 i,j;
	x*=3;
	y<<=3;
	len*=3;
	for(i=0;i<len;i++)
	{
		for(j=0;j<8;j++)
		{
			oledBuff[x+i][y+j]^=0xFF;
		}		
	}
}

void OledDrawInt(u8 x,u8 y,s32 v,u8 ml,u8 f)
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
	if(len>ml)
	{
		len=ml-1;
		while(len>=p)
		{
			buf[len]='9';
			len--;
		}
		OledDrawString(x,y,buf,f);	
		return;
	}
	while(ml>len)
	{
		buf[ml-1]=' ';
		ml--;
	}
	ml--;
	len=ml;
	while(len>=p)
	{
		buf[len]=v%10+'0';
		v/=10;
		len--;
	}
	OledDrawString(x,y,buf,f);	
}

void OledDrawFixed(u8 x,u8 y,s32 v,s8 pre,u8 ml,u8 f)
{
	s8 buf[14],p=0,len=0;
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
	len++;
	if(len>ml)
	{
		len=ml-1;
		while(len>=p)
		{
			buf[len]='9';
			len--;
		}
		buf[ml-pre-1]='.';
		OledDrawString(x,y,buf,f);	
		return;
	}
	while(ml>len)
	{
		buf[ml-1]=' ';
		ml--;
	}
	if(ml<pre+2+p)
		ml=pre+2+p;
	ml--;
	
	len=ml;
	
	while(len>=p)
	{
		if(len+pre==ml)
		{
			buf[len]='.';
		}
		else
		{
			buf[len]=v%10+'0';
			v/=10;
		}
		len--;
	}
	OledDrawString(x,y,buf,f);	
}

//vertical scan
void OledDrawBitmap(u8 x,u8 y,u8 w,u8 h,const u8* data)
{
	u8 px,py;
	u8 bh=h>>3;
	u32 pos=0;
	for(px=x;px<w+x;px++)
	{
		for(py=y;py<y+h;py++)
		{
			if(px&1)
			{
				oledBuff[px>>1][py]&=0xF0;
				if(data[pos>>3]&(0x80>>(pos&0x07)))				
					oledBuff[px>>1][py]|=0x0F;				
			}
			else
			{
				oledBuff[px>>1][py]&=0x0F;
				if(data[pos>>3]&(0x80>>(pos&0x07)))				
					oledBuff[px>>1][py]|=0xF0;
			}
			pos++;
		}
	}	
}
