#include "spi.h"

u8 spi_init_flag;

void SPIInit(void)
{
	spi_init_flag=0;
}

void SPI1Init(void)
{
	if(spi_init_flag&0x01)
		return;	
	
	spi_init_flag|=0x01;
}

void SPI2Init(void)
{
	if(spi_init_flag&0x02)
		return;
	
	spi_init_flag|=0x02;
}

void SPI3Init(void)
{
	if(spi_init_flag&0x04)
		return;
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//SPI3 SCK->PB3,MISO->PB4,MOSI->PB5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gi);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);
	
	si.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	si.SPI_Mode=SPI_Mode_Master;
	si.SPI_DataSize=SPI_DataSize_8b;
	si.SPI_CPOL=SPI_CPOL_High;
	si.SPI_CPHA=SPI_CPHA_2Edge;
	si.SPI_NSS=SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
	si.SPI_FirstBit=SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial=7;
	SPI_Init(SPI3,&si);
	SPI_Cmd(SPI3,ENABLE);
	spi_init_flag|=0x04;
}

void SPIWrite(SPI_TypeDef* spi,u8 b)
{
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(spi,b);
	while(SPI_I2S_GetFlagStatus(spi,SPI_I2S_FLAG_BSY)==SET);	
}

u8 SPIRead(SPI_TypeDef* spi)
{
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(spi,0xFF);
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);	
	u8 b=SPI_I2S_ReceiveData(spi);	
	while(SPI_I2S_GetFlagStatus(spi,SPI_I2S_FLAG_BSY)==SET);	
	return b;
}

u8 SPIReadWrite(SPI_TypeDef* spi,u8 b)
{
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(spi,b);
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);	
	u8 res=SPI_I2S_ReceiveData(spi);	
	while(SPI_I2S_GetFlagStatus(spi,SPI_I2S_FLAG_BSY)==SET);	
	return res;
}

void SPIClearBuff(SPI_TypeDef* spi)
{
	SPI_I2S_ReceiveData(spi);
}
