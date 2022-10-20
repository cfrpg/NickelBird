#include "pages.h"
#include "ads1256.h"
#include "adc.h"
u32 EOcnt;

void UpdateADC(void)
{
	u8 i;
	//ADSReadAllChannel(sys.ADCRawData);
	for(i=0;i<8;i++)
	{		
		//sys.sensors.ADCData[i]=((float)sys.ADCRawData[i])/(1<<23)*5;
		sys.sensors.ADCData[i]+=1;
	}
	//ADCReadVol(&(sys.sensors.ADCData[8]));	
}

void SendSensorData(void)
{
	//sys.sensors.data[0]=sys.rpm;
	
	LinkSendData(&sys.sensors,sizeof(SensorDataPackage));
}

void UpdateSensors(void)
{
	//if(sys.adcEn)
	//printf("1\r\n");
	//	UpdateADC();
	//printf("2\r\n");
	SendSensorData();
}

void TIM8_BRK_TIM12_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM12, TIM_IT_Update) != RESET)
	{		
		TIM_ClearITPendingBit(TIM12, TIM_IT_Update);
		sys.rpm=EOcnt*60/4;
		//printf("EO %d\r\n",EOcnt);
		EOcnt=0;
		
	}
}

void EXTI4_IRQHandler(void)
{
	//EXTI_ClearITPendingBit(EXTI_Line4);
	delay_us(20);
	if(PDin(4)==0)
	{
		delay_us(20);
		if(PDin(4)==0)
		{
			EOcnt++;
			//printf("EO %d\r\n",EOcnt);	
		}
		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line4);
}

