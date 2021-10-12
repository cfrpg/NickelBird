#include "sensors.h"
#include "pages.h"
#include "i2c.h"
#include "ms4525do.h"
#include "spi.h"
#include "bmp280.h"
#include "sdp3x.h"
#include "extin.h"
#include "pwm.h"
#include "pages.h"
#include "extin.h"

u8 sensMSASEnabled;
float sensMS4525DP;

void SeneorsInit(void)
{

}

void SensorsSlowUpdate(void)
{	
	if(sys.slowUpdate!=0)
	{
		(*sys.slowUpdate)();
	}
}

void SensorsFastUpdate(void)
{
	if(sys.fastUpdate!=0)
	{
		(*sys.fastUpdate)();
	}
}

void SensorsIntUpdate(void)
{
	if(sys.intUpdate!=0)
	{
		(*sys.intUpdate)();
	}
}
