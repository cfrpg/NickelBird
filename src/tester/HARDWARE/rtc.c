#include "rtc.h"
#include "delay.h"
#include "stdio.h"
#include "pages.h"

DateTime time;
u16 RTCcnt;

void RTCSetWakeUp(u32 wks,u16 cnt)
{
	EXTI_InitTypeDef ei;
	NVIC_InitTypeDef ni;
	
	RTC_WakeUpCmd(DISABLE);	
	RTC_WakeUpClockConfig(wks);	
	RTC_SetWakeUpCounter(cnt);	
	
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	RTC_WakeUpCmd(ENABLE);
	
	ei.EXTI_Line = EXTI_Line22;
	ei.EXTI_Mode = EXTI_Mode_Interrupt;
	ei.EXTI_Trigger = EXTI_Trigger_Rising; 
	ei.EXTI_LineCmd = ENABLE;
	EXTI_Init(&ei); 
 
	ni.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	ni.NVIC_IRQChannelPreemptionPriority = 0x01;
	ni.NVIC_IRQChannelSubPriority = 0x02;
	ni.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&ni);
	RTCcnt=0;
}

u8 RTCInit()
{
	u8 t;
	RTC_InitTypeDef ri;	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		t=0;
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET&&t<250)
		{
			t++;
			delay_ms(10);
		}
		if(t>=250)
		{
			printf("rtc error\r\n");
			return 1;
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		ri.RTC_AsynchPrediv=0x7F;
		ri.RTC_SynchPrediv=0xFF;
		ri.RTC_HourFormat=RTC_HourFormat_24;
		RTC_Init(&ri);
		
		RTCSetTime(12,0,0);
		RTCSetDate(19,5,1);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);
	}
	RTCSetWakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);
	
	return 0;
}

ErrorStatus RTCSetTime(u8 h,u8 m,u8 s)
{
	RTC_TimeTypeDef rt;
	rt.RTC_Hours=h;
	rt.RTC_Minutes=m;
	rt.RTC_Seconds=s;
	rt.RTC_H12=RTC_H12_AM;
	return RTC_SetTime(RTC_Format_BIN,&rt);
}

ErrorStatus RTCSetDate(u8 y,u8 m,u8 d)
{
	u16 w;
	RTC_DateTypeDef rd;
	rd.RTC_Year=y;
	rd.RTC_Month=m;
	rd.RTC_Date=d;
	
	w=y+(y>>2)-37+26*((u16)m+1)/10+d-1;
	rd.RTC_WeekDay=w%7;
	return RTC_SetDate(RTC_Format_BIN,&rd);
}

void RTCReadTime(void)
{
	RTC_TimeTypeDef rt;
	RTC_DateTypeDef rd;
	RTC_GetDate(RTC_Format_BIN,&rd);
	RTC_GetTime(RTC_Format_BIN,&rt);
	time.year=rd.RTC_Year;
	time.month=rd.RTC_Month;
	time.day=rd.RTC_Date;
	time.hour=rt.RTC_Hours;
	time.min=rt.RTC_Minutes;
	time.sec=rt.RTC_Seconds;
	time.ms=RTCcnt;
}

void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP中断?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//清除中断标志
		RTCcnt=0;
		//PagesDrawStatusBar();		
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22的中断标志 								
}
 
