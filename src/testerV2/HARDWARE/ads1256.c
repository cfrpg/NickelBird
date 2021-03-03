#include "ads1256.h"
#include "delay.h"
#include "stdio.h"

s32 ADSBuff[8];
u8 ADSCurrCh;

u16 ADSCpu;
u16 ADSCpu2;
void ADS1256Init(void)
{
	u8 i;
	GPIO_InitTypeDef gi;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD,ENABLE);
	//Init SPI GPIO
	gi.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_OType=GPIO_OType_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&gi);
	gi.GPIO_Pin=GPIO_Pin_11;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gi);
	//Init common GPIO
	gi.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_1|GPIO_Pin_2;
	gi.GPIO_Mode=GPIO_Mode_OUT;
	gi.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&gi);
	gi.GPIO_Pin=GPIO_Pin_0;
	gi.GPIO_Mode=GPIO_Mode_IN;
	gi.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&gi);
	
	EXTI_InitTypeDef ei;
	NVIC_InitTypeDef ni;
	
	for(i=0;i<8;i++)
	{
		ADSBuff[i]=0;
	}

	ADSCurrCh=0;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource0);
	
	ei.EXTI_Line=EXTI_Line0;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI0_IRQn;
	ni.NVIC_IRQChannelCmd=ENABLE;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&ni);
	
	ADS_CS=0;
	ADS_SYNC=1;
	ADS_RST=1;
	ADS_CS=1;
	ADS_SCK=0;
	ADS_DI=1;	
}

void ADSDelaySCK(void)
{
	u8 i;
	for(i=0;i<10;i++);
}

void ADSDelayData(void)
{
	delay_us(9);
}

void ADSSendByte(u8 b)
{
	u8 i;
	ADSDelaySCK();
	ADSDelaySCK();
	for(i=0;i<8;i++)
	{
		if(b&0x80)
			ADS_DI=1;
		else
			ADS_DI=0;
		ADS_SCK=1;
		ADSDelaySCK();
		b<<=1;
		ADS_SCK=0;
		ADSDelaySCK();
	}
}

u8 ADSReceiveByte(void)
{
	u8 i;
	u8 b=0;
	ADSDelaySCK();
	for(i=0;i<8;i++)
	{
		ADS_SCK=1;
		ADSDelaySCK();
		b<<=1;
		ADS_SCK=0;
		if(ADS_DO)
			b|=1;
		ADSDelaySCK();
	}
	return b;
}

void ADSWriteReg(u8 id,u8 v)
{
	ADS_CS=0;
	ADSSendByte(ADS_CMD_WREG|id);
	ADSSendByte(0x00);
	ADSSendByte(v);
	ADS_CS=1;
}

u8 ADSReadReg(u8 id)
{
	u8 b;
	ADS_CS=0;
	ADSSendByte(ADS_CMD_RREG|id);
	ADSSendByte(0);
	ADSDelayData();
	b=ADSReceiveByte();
	ADS_CS=1;
	return b;
}

void ADSSendCmd(u8 cmd)
{
	ADS_CS=0;
	ADSSendByte(cmd);
	ADS_CS=1;
}

void ADSStartUp(u8 gain,u8 rate)
{
	u8 buf[4]={0};
	//STATUS:ORDER,ACAL,BUFEN
	buf[0]=(0<<3)|(1<<2)|(0<<1);
	//MUX
	buf[1]=0x08;
	//ADCON
	buf[2]=(0<<5)|(0<<3)|(gain);
	//DRATE
	buf[3]=rate;
	ADS_CS=0;
	ADSSendByte(ADS_CMD_WREG|0);
	ADSSendByte(0x03);
	ADSSendByte(buf[0]);
	ADSSendByte(buf[1]);
	ADSSendByte(buf[2]);
	ADSSendByte(buf[3]);
	ADS_CS=1;
	delay_ms(10);
	ADSSendCmd(ADS_CMD_SELFCAL);
	delay_ms(100);	
}

void ADSSetChannel(u8 ch)
{
	ADSWriteReg(ADS_REG_MUX,(ch<<4)|0x08);
}

void ADSSetDiffChannel(u8 ch)
{
	ADSWriteReg(ADS_REG_MUX,((ch<<1)<<4)|((ch<<1)-1));
}

void ADSWaitDRDYLow(void)
{	
	u32 t=0;
	while(ADS_DRDY)
	{
		t++;
		if(t>0x00300000)
		{
			printf("ADS DRDY Time out\r\n");
			break;
		}
	}
	//printf("%d\r\n",t);
}

void ADSWaitDRDYHigh(void)
{	
	u32 t=0;
	while(!ADS_DRDY)
	{
		t++;
		if(t>0x00300000)
		{
			printf("ADS DRDY Time out\r\n");
			break;
		}
	}
	//printf("%d\r\n",t);
}

s32 ADSReadData(void)
{
	u32 data=0;
	u32 t;
	ADS_CS=0;
	ADSSendByte(ADS_CMD_RDATA);
	ADSDelayData();	
	t=ADSReceiveByte();
	data|=(t<<16);
	ADSDelayData();	
	t=ADSReceiveByte();
	data|=(t<<8);
	ADSDelayData();	
	t=ADSReceiveByte();
	data|=(t);
	ADS_CS=1;
	if(data&0x800000)
		data+=0xFF000000;
	return (s32)data;
}

s32 ADSReadChannel(u8 ch)
{
	ADSWaitDRDYHigh();
	ADSWaitDRDYLow();	
	ADSSetChannel(ch);
	delay_us(5);	
	ADSSendCmd(ADS_CMD_SYNC);
	delay_us(5);	
	ADSSendCmd(ADS_CMD_WAKEUP);
	delay_us(25);
	ADSReadData();
	ADSWaitDRDYHigh();
	ADSWaitDRDYLow();
	return ADSReadData();	
}

s32 ADSReadCurrChannel(void)
{
	ADSWaitDRDYLow();
	ADSSendCmd(ADS_CMD_SYNC);
	delay_us(5);
	ADSSendCmd(ADS_CMD_WAKEUP);
	delay_us(5);
	return ADSReadData();	
}

void ADSReadAllChannel(s32 data[])
{
	u8 i;	
	for(i=0;i<8;i++)
		data[i]=ADSBuff[i];	
}

void ADSIRQHandler(void)
{
//	u16 t;
	ADSSetChannel(ADSCurrCh);
	delay_us(5);	
	ADSSendCmd(ADS_CMD_SYNC);
	delay_us(5);	
	ADSSendCmd(ADS_CMD_WAKEUP);
	delay_us(25);
	if(ADSCurrCh==0)
	{
		ADSBuff[7]=ADSReadData();
		ADSCpu2=(cpucnt+10000-ADSCpu)%10000;
		//printf("%d\r\n",ADSCpu);
		ADSCpu=cpucnt;
	}
	else		
	{
		ADSBuff[ADSCurrCh-1]=ADSReadData();
	}
	ADSCurrCh++;
	ADSCurrCh&=0x07;
}

void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);		
		ADSIRQHandler();		
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
