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

void SensorsSlowUpdate(u16 time)
{	
	if(sys.slowUpdate!=0)
	{
		(*sys.slowUpdate)(time);
	}
}

void SensorsFastUpdate(u16 time)
{
	if(sys.fastUpdate!=0)
	{
		(*sys.fastUpdate)(time);
	}
}

void SensorsIntUpdate(void)
{
	if(sys.intUpdate!=0)
	{
		(*sys.intUpdate)();
	}
}
