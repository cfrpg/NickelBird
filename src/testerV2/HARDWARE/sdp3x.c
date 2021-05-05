#include "sdp3x.h"
#include "i2c.h"
#include "math.h"
#include "stdio.h"

float sdp3xOffset;

void Sdp3xInit(u8 addr)
{
	//I2CInitPort(id);
	sdp3xOffset=0;
	
}
void Sdp3xSetMode(u8 addr,u16 cmd)
{
	u8 h=cmd>>8;
	u8 l=cmd&0xFF;
	u8 b=0;
	I2CStart();
	I2CSendByte(addr);
	b=I2CWaitAck();
	I2CSendByte(h);
	I2CWaitAck();
	I2CSendByte(l);
	I2CWaitAck();
	I2CStop();
	//printf("%d\r\n",b);
}

void Sdp3xReadOut(u8 addr,u8 n,s16 data[])
{
	u8 b;
	u8 crc;
	u8 i;
	I2CStart();
	I2CSendByte(addr+1);
	I2CWaitAck();
	for(i=0;i<n;i++)
	{
		if(i!=0)
			I2CAck();
		data[i]=I2CReadByte();
		I2CAck();
		data[i]<<=8;
		data[i]+=I2CReadByte();
		I2CAck();
		crc=I2CReadByte();		
	}
	I2CNAck();
	I2CStop();
}

float Sdp3xReadAirspeed(u8 addr,float temp,float pre,float *dpre)
{
	s16 data;
	u8 f=0;
	float ias;
	Sdp3xReadOut(addr,3,&data);
	float dp=((float)data)/60;
	dp-=sdp3xOffset;
	if(dp<0)
	{
		dp*=-1;
		f=1;
	}
	if(temp==0||pre==0)
		ias=sqrt(2*dp/1.125);
	else
		ias=96600/pre*sqrt(2*dp/1.1289*temp+(273.15)/298.15);
	
	if(f)
		return -ias;
	*dpre=dp;
	return ias;
}

void Sdp3xCali(u8 addr,u8 n)
{
	s32 sum=0;
	s16 data;
	while(n--)
	{
		Sdp3xReadOut(addr,1,&data);
		sum+=data;
	}
	sdp3xOffset=(float)sum/n;
	sdp3xOffset/=60;
	printf("sdp3x offset:%f\r\n",sdp3xOffset);
}

void Sdp3xReadDevID(u8 addr)
{
	u8 b,i,j;
	u8 buf[18];
	I2CStart();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(0x36);
	I2CWaitAck();
	I2CSendByte(0x7C);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(0xE1);
	I2CWaitAck();
	I2CSendByte(0x02);
	I2CWaitAck();
	I2CStop();
	I2CStart();
	I2CSendByte(addr+1);
	I2CWaitAck();
	for(i=0;i<18;i++)
	{
		if(i!=0)
			I2CAck();
		buf[i]=I2CReadByte();		
	}
	I2CNAck();
	I2CStop();
	for(i=0;i<18;i++)
	{
		//if(i%3!=2)
		{
			printf("%x ",buf[i]);
		}
	}
	printf("\r\n");
}
