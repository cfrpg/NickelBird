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
	sys.intEnabled[0]=0;
	sys.intEnabled[1]=-1;
}

void SensorsSlowUpdate(void)
{	
	float dp;
	//Available sensor data
	sys.temperature=BMPReadTemperature();//Temperature
	sys.pressure=BMPReadPressure();//atm
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
}

void SensorsFastUpdate(void)
{
	float tmp;
	if(sensMSASEnabled)
		MS4525DOReadAirspeed(sys.rho,&sensMS4525DP);
//	sys.sensors.SensorData[0]=0;
	sys.sensors.SensorData[1]=sys.freq[0];
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
