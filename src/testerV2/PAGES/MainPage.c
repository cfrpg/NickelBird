#include "pages.h"
#include "oled.h"
#include "pwm.h"

void mainpage_showData(u8 f);
	
struct
{
	u8 state;
	u8 currch;
	u8 bind;
} mainp;
void PageInit_main(u8 f)
{
	if(f)
	{
		mainp.state=0;
		mainp.currch=0;
		mainp.bind=0;
		return;
	}
	OledClear(0);
	PagesDrawHeader(MainPage,"Main");
	
	if(PWMIsArmed())
	{
		OledDispString(0,2,"ARMED",0);
	}
	
	OledDispString(3,4,":      |    %",0);
	OledDispString(3,6,":      |    %",0);
		
	OledDispString(0,15,"SWAP",0);
	
	OledDispString(17,15,"CALI",0);
	
	mainpage_showData(0xFF);
	
	
}

void PageUpdate_main(void)
{
	u8 key=currKey&(currKey^lastKey);
	u8 wheelpush=0;
	u8 draw=0;
	u8 i;
	s16 dpwm=0;
	if((currWheelPush&0x07)==0x01)
		wheelpush=1;
	if(mainp.state==0)
	{
		//normal state
		if(key&KEY_A)
		{
			mainp.currch^=1;
			draw|=0x01;
		}
		if(key&KEY_B)
		{
			mainp.bind^=1;
			draw|=0x01;
		}
		if(PWMIsArmed())
		{
			if(wheelpush)
			{
				PWMDisarm();
				sys.pwm[0]=params.pwm_disarmed;
				sys.pwm[1]=params.pwm_disarmed;
				draw|=0x06;
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
				if(mainp.bind)
				{
					sys.pwm[0]+=dpwm;
					sys.pwm[1]+=dpwm;
					draw|=0x06;
				}
				else
				{
					if(mainp.currch)
					{
						sys.pwm[1]+=dpwm;
						draw|=0x04;
						
					}
					else
					{
						sys.pwm[0]+=dpwm;
						draw|=0x02;
					}
				}
			}
			
			for(i=0;i<2;i++)
			{
				if(sys.pwm[i]<params.pwm_min)
					sys.pwm[i]=params.pwm_min;
				if(sys.pwm[i]>params.pwm_max)
					sys.pwm[i]=params.pwm_max;
			}
			PWMSet(sys.pwm[0],sys.pwm[1]);
			
		}
		else
		{
			if(wheelpush)
			{
				PWMArm();
				sys.pwm[0]=params.pwm_min;
				sys.pwm[1]=params.pwm_min;
				draw|=0x06;
				OledDispString(0,2,"ARMED",0);
			}
		}
		if(key&KEY_LEFT)
			PagesNext(-1);
		if(key&KEY_RIGHT)
			PagesNext(1);
	}
	mainpage_showData(draw);
}

void mainpage_showData(u8 f)
{
	//OledDispInt(0,8,currWheel,5,0);
	//OledDispInt(0,9,currKey,3,0);
	if(f&0x01)
	{
		if(mainp.bind)
		{
			OledDispString(0,4,"CH1",1);
			OledDispString(0,6,"CH2",1);
			OledDispString(8,15,"BIND",1);
		}
		else
		{
			OledDispString(8,15,"BIND",0);
			if(mainp.currch)
			{
				OledDispString(0,4,"CH1",0);
				OledDispString(0,6,"CH2",1);
			}
			else
			{
				OledDispString(0,4,"CH1",1);
				OledDispString(0,6,"CH2",0);
			}
		}
	}
	if(f&0x02)
	{
		OledDispInt(4,4,sys.pwm[0],4,0);
		OledDispInt(11,4,(sys.pwm[0]-1000)/10,4,0);
	}
	if(f&0x04)
	{
		OledDispInt(4,6,sys.pwm[1],4,0);
		OledDispInt(11,6,(sys.pwm[1]-1000)/10,4,0);
	}
}
