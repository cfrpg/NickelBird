#include "i2c.h"
#include "delay.h"

void I2CInit(void)
{
	GPIO_InitTypeDef gi;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_OD;
	gi.GPIO_Speed=GPIO_Speed_100MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&gi);
	I2C_SCL_OUT=1;
	I2C_SDA_OUT=1;
	//I2CStop();
}

void i2cDelay(void)
{
	delay_us(4);
}

void I2CStart(void)
{
	I2C_OUT();
	I2C_SDA_OUT=1;
	I2C_SCL_OUT=1;
	i2cDelay();
	I2C_SDA_OUT=0;
	i2cDelay();
	I2C_SCL_OUT=0;
	i2cDelay();
}

void I2CStop(void)
{
	I2C_OUT();	
	I2C_SDA_OUT=0;
	I2C_SCL_OUT=1;
	i2cDelay();
	//I2C_SCL_OUT=1;	
	I2C_SDA_OUT=1;
	//i2cDelay();
}

void I2CSendByte(u8 b)
{
	u8 i;
	I2C_OUT();
	//I2C_SCL_OUT=0;
	for(i=0;i<8;i++)
	{
		if(b&0x80)
			I2C_SDA_OUT=1;
		else
			I2C_SDA_OUT=0;
		i2cDelay();
		I2C_SCL_OUT=1;
		i2cDelay();
		I2C_SCL_OUT=0;
		if(i==7)
			I2C_SDA_OUT=1;
		b<<=1;
		i2cDelay();
	}
}

u8 I2CReadByte(void)
{
	u8 i,res;
	I2C_IN();
	res=0;
	//I2C_SCL_OUT=0;
	for(i=0;i<8;i++)
	{
		res<<=1;
		I2C_SCL_OUT=1;
		i2cDelay();
		if(I2C_SDA_IN)
			res|=1;
		I2C_SCL_OUT=0;
		i2cDelay();
	}
	return res;
}

u8 I2CWaitAck(void)
{
	u8 res,t;
	I2C_IN();
	I2C_SDA_OUT=1;
	i2cDelay();
	I2C_SCL_OUT=1;
	i2cDelay();
	t=0;
	res=0;
	while(I2C_SDA_IN)
	{
		t++;
		if(t>250)
		{
			//I2CStop();
			res=1;
			break;
		}
	}
	I2C_SCL_OUT=0;
	i2cDelay();
	return res;
}

void I2CAck(void)
{
	I2C_SCL_OUT=0;
	I2C_OUT();
	I2C_SDA_OUT=0;
	i2cDelay();
	I2C_SCL_OUT=1;
	i2cDelay();
	I2C_SCL_OUT=0;
	i2cDelay();
	I2C_SDA_OUT=1;
}

void I2CNAck(void)
{
	I2C_SCL_OUT=0;
	I2C_OUT();
	I2C_SDA_OUT=1;
	i2cDelay();
	I2C_SCL_OUT=1;
	i2cDelay();
	I2C_SCL_OUT=0;
	i2cDelay();
}

u8 I2CCheckDevice(u8 addr)
{
	
}
