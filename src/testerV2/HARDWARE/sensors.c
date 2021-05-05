#include "sensors.h"
#include "pages.h"
#include "i2c.h"
#include "ms4525do.h"
#include "spi.h"
#include "bmp280.h"
#include "sdp3x.h"
#include "extin.h"

u8 sensMSASEnabled;
float sensMS4525DP;

void SeneorsInit(void)
{
	//Enable or disable MS4525 airspeed sensor.
	//sensMSASEnabled=1;
	//Enable of disable extrenal interrupt source;
	//-1:disable
	//n:store interrupt flag to sys.sensors.SensorData[n]
	sys.intEnabled[0]=-1;
	sys.intEnabled[1]=-1;
	
	Sdp3xInit(SDP3X_ADDR1);
	Sdp3xReadDevID(SDP3X_ADDR1);
	Sdp3xSetMode(SDP3X_ADDR1,0x3615);
	Sdp3xSetMode(SDP3X_ADDR2,0x3615);
	Sdp3xSetMode(SDP3X_ADDR3,0x3615);
}

void SensorsSlowUpdate(void)
{	
	float dp;
	s16 tmp[2];
	//Available sensor data
//	sys.temperature=BMPReadTemperature();//Temperature
//	sys.pressure=BMPReadPressure();//atm
//	sys.rho;//rho at startup
//	BMPReadAverageRho(1);//read new rho
	
//	MS4525DOReadAverageAirspeed();//Average-Until-Read airspeed from MS4525DO
//	MS4525DOReadAirspeed(sys.rho,&sensMS4525DP);//Airspeed from MS4525DO
	
//	Sdp3xReadAirspeed(SDP3X_ADDR1,sys.temperature,sys.pressure,&dp);//Airspeed from SDP3x,with dp
	
	
	
//	sys.freq[0];//External interrupt freq 0
//	sys.freq[1];//External interrupt freq 1
	
	//Set data
//	sys.sensors.SensorData[0]=MS4525DOReadAirspeed(sys.rho,&sensMS4525DP);
//	sys.sensors.SensorData[1]=sensMS4525DP;
//	sys.sensors.SensorData[2]=sys.pressure;
//	sys.sensors.SensorData[3]=sys.temperature;
//	sys.sensors.SensorData[0]=1;
//	sys.sensors.SensorData[1]=2;
//	sys.sensors.SensorData[2]=3;
//	sys.sensors.SensorData[3]=4;
}

void SensorsFastUpdate(void)
{
	//float tmp;
	
	s16 tmp[2];
	Sdp3xReadOut(SDP3X_ADDR1,2,tmp);
	sys.sensors.SensorData[0]=((float)tmp[0])/60.0;
	sys.sensors.SensorData[3]=((float)tmp[1])/200;
	Sdp3xReadOut(SDP3X_ADDR2,1,tmp);
	sys.sensors.SensorData[1]=((float)tmp[0])/60.0;
	Sdp3xReadOut(SDP3X_ADDR3,1,tmp);
	sys.sensors.SensorData[2]=((float)tmp[0])/60.0;
	if(sensMSASEnabled)
		MS4525DOReadAirspeed(sys.rho,&sensMS4525DP);
//	sys.sensors.SensorData[0]=0;
//	sys.sensors.SensorData[1]=sys.freq[0];
//	sys.sensors.SensorData[2]=0;
//	sys.sensors.SensorData[3]=0;
}

void SensorsIntUpdate(void)
{
		if(sys.intFlag[0]&&sys.intEnabled[0]>=0)
		{
			sys.sensors.SensorData[sys.intEnabled[0]]=1;
			sys.intFlag[0]=0;
			//sys.sensors.SensorData[0]=sys.freq[0];
		}
		if(sys.intFlag[1]&&sys.intEnabled[1]>=0)
		{
			sys.sensors.SensorData[sys.intEnabled[1]]=1;
			sys.intFlag[1]=0;
			//sys.sensors.SensorData[1]=sys.freq[1];
		}
}
