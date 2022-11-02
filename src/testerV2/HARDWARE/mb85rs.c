#include "mb85rs.h"

void FRAMInit(void)
{
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	//Init SPI GPIO
	gi.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	gi.GPIO_Mode=GPIO_Mode_AF;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&gi);
	//Init common GPIO
	gi.GPIO_Pin=GPIO_Pin_12;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&gi);	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	
	//Init SPI2
	si.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
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
	
	FRAM_CS=1;
}

void FRAMSendByte(u8 b)
{		
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);		
	SPI_I2S_SendData(SPI2,b);	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)==SET);
}

u8 FRAMReceiveByte(void)
{	
//	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);	
//	SPI_I2S_SendData(SPI2,0xFF);
//	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);	
//	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)==SET);	
//	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);	
//	u8 b= SPI_I2S_ReceiveData(SPI2);
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);	
	SPI_I2S_SendData(SPI2,0xFF);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);	
	u8 b=SPI_I2S_ReceiveData(SPI2);	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)==SET);	
	return b;

}

void FRAMWrite(u32 addr,u32 len,u8* data)
{
	u8 addl,addm,addh;
	u32 i;
	//Cmd WREN
	FRAM_CS=0;
	FRAMSendByte(FRAM_WREN);
	FRAM_CS=1;
#ifdef FRAM_24BIT_ADDR
	addl=(u8)(addr&0x00FF);
	addm=(u8)((addr>>8)&0x00FF);
	addh=(u8)((addr>>16)&0x00FF);
	//Cmd WRITE
	FRAM_CS=0;
	FRAMSendByte(FRAM_WRITE);
	FRAMSendByte(addh);
	FRAMSendByte(addm);
	FRAMSendByte(addl);
#else
	addl=(u8)(addr&0x00FF);
	addh=(u8)((addr>>8)&0x00FF);
	//Cmd WRITE
	FRAM_CS=0;
	FRAMSendByte(FRAM_WRITE);
	FRAMSendByte(addh);
	FRAMSendByte(addl);
#endif
	
	for(i=0;i<len;i++)
	{
		FRAMSendByte(data[i]);
	}
	FRAM_CS=1;
	FRAM_CS=1;
	//Cmd WRDI
	FRAM_CS=0;
	FRAMSendByte(FRAM_WRDI);
	FRAM_CS=1;
}

void FRAMRead(u32 addr,u32 len,u8* data)
{
	u8 addl,addm,addh;
	u32 i;
#ifdef FRAM_24BIT_ADDR
	addl=(u8)(addr&0x00FF);
	addm=(u8)((addr>>8)&0x00FF);
	addh=(u8)((addr>>16)&0x00FF);
	//Cmd READ
	FRAM_CS=0;	
	FRAMSendByte(FRAM_READ);
	FRAMSendByte(addh);
	FRAMSendByte(addm);
	FRAMSendByte(addl);	
#else
	addl=(u8)(addr&0x00FF);
	addh=(u8)((addr>>8)&0x00FF);
	//Cmd READ
	FRAM_CS=0;	
	FRAMSendByte(FRAM_READ);
	FRAMSendByte(addh);
	FRAMSendByte(addl);	
#endif
	SPI_I2S_ReceiveData(SPI2);
	for(i=0;i<len;i++)
	{
		data[i]=FRAMReceiveByte();
	}
	FRAM_CS=1;
}
