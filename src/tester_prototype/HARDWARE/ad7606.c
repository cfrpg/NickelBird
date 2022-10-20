#include "ad7606.h"
#include "stdio.h"
#include "delay.h"
 
void AD7606Init(void)
{
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);	
	
	//CS,RST
	gi.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_8;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOG,&gi);
	
	//CV
	gi.GPIO_Pin=GPIO_Pin_5;
	GPIO_Init(GPIOB,&gi);
		
	//BUSY
	gi.GPIO_Pin=GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(GPIOG,&gi);
	
	//SPI3 SCK->PC10,MISO->C11
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&gi);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);
	
	si.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	si.SPI_Mode=SPI_Mode_Master;
	si.SPI_DataSize=SPI_DataSize_8b;
	si.SPI_CPOL=SPI_CPOL_High;
	si.SPI_CPHA=SPI_CPHA_2Edge;
	si.SPI_NSS=SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
	si.SPI_FirstBit=SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial=7;
	SPI_Init(SPI1,&si);
	SPI_Cmd(SPI1,ENABLE);
	AD7606Reset();
	ADI_CV=1;
}

u8 ADRead(void)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(SPI1,0x0f);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	
	u8 b=SPI_I2S_ReceiveData(SPI1);
	
	return b;
}

void ADRead2(u8* ptr)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(SPI1,0x0f);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	
	*ptr=SPI_I2S_ReceiveData(SPI1);
}



void AD7606Reset(void)
{
	ADI_RST=0;
	ADI_RST=1;
	ADI_RST=1;
	ADI_RST=1;
	ADI_RST=1;
	ADI_RST=0;
}

void AD7606Start(void)
{
	u8 i;
	ADI_CV=0;
	ADI_CV=0;
	ADI_CV=0;
	//delay_us(1);
	ADI_CV=1;
}

void AD7606Read(s16* data)
{
	u8 i;
	AD7606Start();	
	while(ADI_BUSY!=0);	
	ADI_CS=0;
	for(i=0;i<8;i++)
	{
		data[i]=ADRead();
		data[i]<<=8;
		data[i]|=ADRead();
	}
	ADI_CS=1;
	
}
