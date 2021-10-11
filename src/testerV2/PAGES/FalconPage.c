#include "pages.h"
#include "oled.h"
#include "pwm.h"

void falconpage_showData(u8 f);
void falconpage_fastUpdate(void);
void falconpage_intUpdate(void);

struct
{
	const s32* pwm_min;
	const s32* pwm_max;
	const s32* pwm_disarmed;
	u8 state;
	u8 currch;
	u8 bind;
	u8 phase;
	u32 lasttime;
	s32 phaseTime[2];
	u16 pwm[2];
	u8 draw;
} falconp;
void PageInit_falcon(u8 f)
{
	if(f)
	{
		falconp.state=0;
		falconp.currch=0;
		falconp.bind=0;
		falconp.phase=0;
		falconp.lasttime=0;
		falconp.phaseTime[0]=0;
		falconp.phaseTime[1]=0;
		falconp.pwm_min=ParamGetFromName("PWM_MIN");
		falconp.pwm_max=ParamGetFromName("PWM_MAX");		
		falconp.pwm_disarmed=ParamGetFromName("PWM_DISARMED");
		falconp.pwm[0]=*falconp.pwm_disarmed;
		falconp.pwm[1]=*falconp.pwm_disarmed;
		return;
	}
	
	sys.slowUpdate=0;
	sys.fastUpdate=falconpage_fastUpdate;
	sys.intUpdate=falconpage_intUpdate;
	
	
	OledClear(0);
	PagesDrawHeader(FalconPage,"Falcon");
	
	if(PWMIsArmed())
	{
		OledDispString(0,2,"ARMED",0);
	}
	
	OledDispString(5,4,":      |    %",0);
	OledDispString(6,5,"    ms|     Hz",0);
	OledDispString(5,7,":      |    %",0);
	OledDispString(6,8,"    ms|     Hz",0);	
	OledDispString(0,10,"OUT  :      |    %",0);
	OledDispString(6,11,"    ms|     Hz",0);	
	
		
	OledDispString(0,15,"SWAP",0);
	
	OledDispString(17,15,"CALI",0);
	
	falconpage_showData(0xFF);
	
	
}

void PageUpdate_falcon(void)
{
	u8 key=currKey&(currKey^lastKey);
	u8 wheelpush=0;	
	u8 i;
	s16 dpwm=0;
	falconp.draw&=0xF0;
	if((currWheelPush&0x07)==0x01)
		wheelpush=1;
	if(falconp.state==0)
	{
		//normal state
		if(key&KEY_A)
		{
			falconp.currch^=1;
			falconp.draw|=0x01;
		}
		if(key&KEY_B)
		{
			falconp.bind^=1;
			falconp.draw|=0x01;
		}
		if(PWMIsArmed())
		{
			if(wheelpush)
			{
				PWMDisarm();
				falconp.pwm[0]=*falconp.pwm_disarmed;
				falconp.pwm[1]=*falconp.pwm_disarmed;
				falconp.draw|=0x06;
				OledDispString(0,2,"      ",0);
			}
			if(key&KEY_UP)
			{
				dpwm=100;
			}
			if(key&KEY_DOWN)
			{
				dpwm=-100;
			}
			if(currWheel>lastWheel)
			{
				dpwm=10;
			}
			if(currWheel<lastWheel)
			{
				dpwm=-10;
			}
			if(dpwm)
			{
				if(falconp.bind)
				{
					falconp.pwm[0]+=dpwm;
					falconp.pwm[1]+=dpwm;
					falconp.draw|=0x06;
				}
				else
				{
					if(falconp.currch)
					{
						falconp.pwm[1]+=dpwm;
						falconp.draw|=0x04;
						
					}
					else
					{
						falconp.pwm[0]+=dpwm;
						falconp.draw|=0x02;
					}
				}
			}
			
			for(i=0;i<2;i++)
			{
				if(falconp.pwm[i]<*falconp.pwm_min)
					falconp.pwm[i]=*falconp.pwm_min;
				if(falconp.pwm[i]>*falconp.pwm_max)
					falconp.pwm[i]=*falconp.pwm_max;
			}
			sys.pwm[0]=falconp.pwm[falconp.phase];
			sys.pwm[1]=falconp.pwm[falconp.phase];
			PWMSet(sys.pwm[0],sys.pwm[1]);
			
		}
		else
		{
			if(wheelpush)
			{
				PWMArm();
				falconp.pwm[0]=*falconp.pwm_min;
				falconp.pwm[1]=*falconp.pwm_min;
				falconp.draw|=0x06;
				OledDispString(0,2,"ARMED",0);
			}
			if(key&KEY_LEFT)
				PagesNext(-1);
			if(key&KEY_RIGHT)
				PagesNext(1);
		}
		
	}
	falconpage_showData(falconp.draw);
}

void falconpage_showData(u8 f)
{
	//OledDispInt(0,8,currWheel,5,0);
	//OledDispInt(0,9,currKey,3,0);
	if(f&0x01)
	{
		if(falconp.bind)
		{
			OledDispString(0,4,"UP",1);
			OledDispString(0,7,"DOWN",1);
			OledDispString(8,15,"BIND",1);
		}
		else
		{
			OledDispString(8,15,"BIND",0);
			if(falconp.currch)
			{
				OledDispString(0,4,"UP  ",0);
				OledDispString(0,7,"DOWN",1);
			}
			else
			{
				OledDispString(0,4,"UP  ",1);
				OledDispString(0,7,"DOWN",0);
			}
		}
	}
	if(f&0x02)
	{
		OledDispInt(6,4,falconp.pwm[0],4,0);
		OledDispInt(13,4,(falconp.pwm[0]-1000)/10,4,0);
		OledDispInt(6,10,sys.pwm[0],4,0);
		OledDispInt(13,10,(sys.pwm[0]-1000)/10,4,0);
	}
	if(f&0x04)
	{
		OledDispInt(6,7,falconp.pwm[1],4,0);
		OledDispInt(13,7,(falconp.pwm[1]-1000)/10,4,0);
		OledDispInt(6,10,sys.pwm[0],4,0);
		OledDispInt(13,10,(sys.pwm[0]-1000)/10,4,0);
	}
	if(f&0x30)
	{
		OledDispInt(6,10,sys.pwm[0],4,0);
		OledDispInt(13,10,(sys.pwm[0]-1000)/10,4,0);
		OledDispInt(6,11,falconp.phaseTime[0]+falconp.phaseTime[1],4,0);
		OledDispFixed(13,11,100000/(falconp.phaseTime[0]+falconp.phaseTime[1]),2,5,0);
	}
	if(f&0x10)
	{
		OledDispInt(6,5,falconp.phaseTime[0],4,0);
		OledDispFixed(13,5,100000/falconp.phaseTime[0],2,5,0);
		f&=0xEF;
	}
	if(f&0x20)
	{
		OledDispInt(6,8,falconp.phaseTime[1],4,0);
		OledDispFixed(13,8,100000/falconp.phaseTime[1],2,5,0);
		f&=0xDF;
	}
}
void falconpage_fastUpdate(void)
{
	
}

void falconpage_intUpdate(void)
{
	if(sys.intFlag[0])
	{
		sys.intFlag[0]=0;
		if(falconp.phase==1)
		{
			falconp.phase=0;
			falconp.phaseTime[1]=sys.intTime[0]-falconp.lasttime;
			falconp.lasttime=sys.intTime[0];
			falconp.draw|=0x20;
			sys.sensors.SensorData[0]++;
		}
	}
	if(sys.intFlag[1])
	{
		sys.intFlag[1]=0;
		if(falconp.phase==0)
		{
			falconp.phase=1;
			falconp.phaseTime[0]=sys.intTime[1]-falconp.lasttime;
			falconp.lasttime=sys.intTime[1];
			falconp.draw|=0x10;
			sys.sensors.SensorData[1]++;
		}
	}
}