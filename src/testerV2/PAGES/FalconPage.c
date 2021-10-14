#include "pages.h"
#include "oled.h"
#include "pwm.h"
#include "math.h"

#define Falcon_Main 0
#define Falcon_Cali_Sel 1
#define Falcon_Cali_Full 2
#define Falcon_Cali_Stroke 3

void falconpage_showData(u8 f);
void falconpage_fastUpdate(void);
void falconpage_intUpdate(void);
void falconpage_intReset(void);

float falconpage_getRawAngle(void);
void falconpage_setPhiCalc(void);
float falconpage_getPhi(float a);

struct
{
	const s32* pwm_min;
	const s32* pwm_max;
	const s32* pwm_disarmed;
	float* aat_sin_scl;
	s32* aat_sin_off;
	float* aat_cos_scl;
	s32* aat_cos_off;
	float* aat_up_ang;
	float* aat_down_ang;
	u32 caliCnt;
	s32 smax,smin,cmax,cmin;
	float angle;
	float phi;
	float phiOffset;
	u8 state;
	u8 currch;
	u8 bind;
	u8 phase;
	u32 lasttime;
	s32 phaseTime[2];
	u16 pwm[2];
	u8 draw;
	s8 strokeDir;
} falconp;
void PageInit_falcon(u8 f)
{
	if(f)
	{
		falconp.state=Falcon_Main;
		falconp.currch=0;
		falconp.bind=0;
		falconp.phase=0;
		falconp.lasttime=0;
		falconp.phaseTime[0]=0;
		falconp.phaseTime[1]=0;
		falconp.pwm_min=ParamGetFromName("PWM_MIN");
		falconp.pwm_max=ParamGetFromName("PWM_MAX");		
		falconp.pwm_disarmed=ParamGetFromName("PWM_DISARMED");
		falconp.aat_sin_scl=ParamGetFromName("AAT_SIN_SCL");
		falconp.aat_sin_off=ParamGetFromName("AAT_SIN_OFF");
		falconp.aat_cos_scl=ParamGetFromName("AAT_COS_SCL");
		falconp.aat_cos_off=ParamGetFromName("AAT_COS_OFF");
		falconp.aat_up_ang=ParamGetFromName("AAT_UP_ANG");
		falconp.aat_down_ang=ParamGetFromName("AAT_DOWN_ANG");
		falconp.pwm[0]=*falconp.pwm_disarmed;
		falconp.pwm[1]=*falconp.pwm_disarmed;
		return;
	}
	
	sys.slowUpdate=0;
	sys.fastUpdate=falconpage_fastUpdate;
	sys.intUpdate=falconpage_intUpdate;
	sys.intReset=falconpage_intReset;
	
	falconpage_setPhiCalc();
	OledClear(0);
	PagesDrawHeader(FalconPage,"Falcon");
	
	if(PWMIsArmed())
	{
		OledDispString(0,2,"ARMED",0);
	}
	
	OledDispString(5,4,":      |    %",0);
	OledDispString(6,5,"    ms|     %",0);
	OledDispString(5,7,":      |    %",0);
	OledDispString(6,8,"    ms|     %",0);	
	OledDispString(0,10,"OUT  :      |    %",0);
	OledDispString(6,11,"    ms|     Hz",0);	
	OledDispString(0,13,"Phi         |",0);	
		
	OledDispString(0,15,"SWAP",0);
	
	OledDispString(17,15,"CALI",0);
	
	falconpage_showData(0xFF);
	
	
}

void falcon_update_main(void)
{
	u8 i;
	s16 dpwm=0;
	//normal state
	falconp.draw|=0x80;
	falconp.draw&=0xf0;
	if(keyPress&KEY_A)
	{
		falconp.currch^=1;
		falconp.draw|=0x01;
	}
	if(keyPress&KEY_B)
	{
		falconp.bind^=1;
		falconp.draw|=0x01;
	}
	if(PWMIsArmed())
	{
		if(wheelPress)
		{
			PWMDisarm();
			falconp.pwm[0]=*falconp.pwm_disarmed;
			falconp.pwm[1]=*falconp.pwm_disarmed;
			falconp.draw|=0x06;
			OledDispString(0,2,"      ",0);
		}
		if(keyPress&KEY_UP)
		{
			dpwm=100;
		}
		if(keyPress&KEY_DOWN)
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
//		sys.pwm[0]=falconp.pwm[falconp.phase];
//		sys.pwm[1]=falconp.pwm[falconp.phase];
//		PWMSet(sys.pwm[0],sys.pwm[1]);
		
	}
	else
	{
		if(wheelPress)
		{
			PWMArm();
			falconp.pwm[0]=*falconp.pwm_min;
			falconp.pwm[1]=*falconp.pwm_min;
			falconp.draw|=0x06;
			OledDispString(0,2,"ARMED",0);
		}
		
		if(keyPress&KEY_C)
		{
			falconp.state=Falcon_Cali_Sel;
			OledClear(0);
			PagesDrawHeader(FalconPage,"Falcon");
			OledDispString(2,3,"Select Cali Type",0);
			OledDispString(0,15,"RANGE",0);	
			OledDispString(9,15,"CALI",1);	
			OledDispString(15,15,"STROKE",0);
			falconp.draw=0;
		}
		if(keyPress&KEY_LEFT)
			PagesNext(-1);
		if(keyPress&KEY_RIGHT)
			PagesNext(1);
		sys.sensors.SensorData[3]=falconp.draw;
	}
}

void falcon_update_cali(void)
{
	if(falconp.state==Falcon_Cali_Sel)
	{
		if(keyPress&KEY_B)
		{
			falconp.state=Falcon_Main;
			PageInit_falcon(0);
		}
		if(keyPress&KEY_A)
		{
			falconp.state=Falcon_Cali_Full;
			falconp.smax=-65535;
			falconp.smin=65535;
			falconp.cmax=-65535;
			falconp.cmin=65535;
			OledDispString(2,5,"Full Range Cali",0);
			OledDispString(7,7,"Min     Max",0);
			OledDispString(2,8,"Sin:      |      ",0);
			OledDispString(2,9,"Cos:      |      ",0);
			OledDispString(0,15,"                     ",0);	
			OledDispString(10,15,"OK",0);	
			falconp.caliCnt=0;
		}
		if(keyPress&KEY_C)
		{
			falconp.state=Falcon_Cali_Stroke;
			falconp.caliCnt=0;
			*falconp.aat_up_ang=0;
			*falconp.aat_down_ang=0;
			OledDispString(0,5,"Stroke Range Cali",0);
			OledDispString(0,7,"Upstroke limit cali",0);
			OledDispString(0,15,"                     ",0);	
			OledDispString(0,13,"Phi",0);
			OledDispString(8,15,"START",0);
		}
	}
	if(falconp.state==Falcon_Cali_Full)
	{		
		if(falconp.smin>sys.sensors.ADCData[6])
			falconp.smin=sys.sensors.ADCData[6];
		if(falconp.smax<sys.sensors.ADCData[6])
			falconp.smax=sys.sensors.ADCData[6];		
		if(falconp.cmin>sys.sensors.ADCData[7])
			falconp.cmin=sys.sensors.ADCData[7];
		if(falconp.cmax<sys.sensors.ADCData[7])
			falconp.cmax=sys.sensors.ADCData[7];
		falconp.caliCnt++;
		OledDispFixed(6,8,(falconp.smin*5000)>>15,3,6,0);
		OledDispFixed(13,8,(falconp.smax*5000)>>15,3,6,0);
		OledDispFixed(6,9,(falconp.cmin*5000)>>15,3,6,0);
		OledDispFixed(13,9,(falconp.cmax*5000)>>15,3,6,0);
//		OledDispInt(2,11,falconp.caliCnt,5,0);
		if(keyPress&KEY_B)
		{
			*falconp.aat_sin_off=(falconp.smax+falconp.smin)>>1;
			*falconp.aat_sin_scl=2.0/(falconp.smax-falconp.smin);
			
			*falconp.aat_cos_off=(falconp.cmax+falconp.cmin)>>1;
			*falconp.aat_cos_scl=2.0/(falconp.cmax-falconp.cmin);
			ParamWrite();
			falconp.state=Falcon_Main;
			PageInit_falcon(0);
		}
	}
	if(falconp.state==Falcon_Cali_Stroke)
	{
		OledDispFixed(6,13,falconp.angle*5730,2,8,0);
		if(falconp.caliCnt==0)
		{
			if(keyPress&KEY_B)
			{
				OledDispString(8,15,"     ",0);
				OledDispString(4,8,"/30",0);
				*falconp.aat_up_ang+=falconpage_getRawAngle();
				falconp.caliCnt++;
			}
		}
		else if(falconp.caliCnt>0&&falconp.caliCnt<30)
		{
			*falconp.aat_up_ang+=falconpage_getRawAngle();
			falconp.caliCnt++;
			OledDispInt(2,8,falconp.caliCnt,2,0);
			if(falconp.caliCnt==30)
			{
				OledDispString(8,8,"OK",0);
				*falconp.aat_up_ang/=30;
				OledDispFixed(11,8,*falconp.aat_up_ang*5730,2,8,0);
				OledDispString(0,9,"Downstroke limit cali",0);				
				OledDispString(8,15,"START",0);
			}
		}
		else if(falconp.caliCnt==30)
		{
			if(keyPress&KEY_B)
			{
				OledDispString(8,15,"     ",0);
				OledDispString(4,10,"/30",0);
				*falconp.aat_down_ang+=falconpage_getRawAngle();
				falconp.caliCnt++;
			}
		}
		else if(falconp.caliCnt>30&&falconp.caliCnt<60)
		{
			*falconp.aat_down_ang+=falconpage_getRawAngle();
			falconp.caliCnt++;
			OledDispInt(2,10,falconp.caliCnt-30,2,0);
			if(falconp.caliCnt==60)
			{
				OledDispString(8,10,"OK",0);	
				*falconp.aat_down_ang/=30;
				OledDispFixed(11,10,*falconp.aat_down_ang*5730,2,8,0);				
				OledDispString(8,15,"SAVE",0);
			}
		}
		else if(falconp.caliCnt==60)
		{
			if(keyPress&KEY_B)
			{				
				ParamWrite();
				falconp.state=Falcon_Main;
				PageInit_falcon(0);
			}
		}
		
		
	}
	
}

void PageUpdate_falcon(void)
{	
	
	if(falconp.state==Falcon_Main)
	{
		falcon_update_main();
		falconpage_showData(falconp.draw);
	}
	else
	{
		falcon_update_cali();
	}
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
		//OledDispFixed(13,5,100000/falconp.phaseTime[0],2,5,0);
		OledDispFixed(13,5,falconp.phaseTime[0]*1000/(falconp.phaseTime[0]+falconp.phaseTime[1]),1,5,0);
		f&=0xEF;
	}
	if(f&0x20)
	{
		OledDispInt(6,8,falconp.phaseTime[1],4,0);
		//OledDispFixed(13,8,100000/falconp.phaseTime[1],2,5,0);		
		OledDispFixed(13,8,falconp.phaseTime[1]*1000/(falconp.phaseTime[0]+falconp.phaseTime[1]),1,5,0);
		f&=0xDF;
	}
	if(f)
	{
		OledDispFixed(4,13,falconp.angle*5730,2,8,0);
		OledDispFixed(13,13,falconp.phi*5730,2,8,0);
	}
}
void falconpage_fastUpdate(void)
{
	if(PWMIsArmed())
	{
		sys.pwm[0]=falconp.pwm[falconp.phase];
		sys.pwm[1]=falconp.pwm[falconp.phase];
		PWMSet(sys.pwm[0],sys.pwm[1]);
	}
}

void falconpage_intUpdate(void)
{
	if(sys.intFlag[1])
	{
		sys.intFlag[1]=0;
		if(falconp.phase==1)
		{
			falconp.phase=0;
			falconp.phaseTime[1]=sys.intTime[1]-falconp.lasttime;
			falconp.lasttime=sys.intTime[1];
			falconp.draw|=0x20;
			sys.sensors.SensorData[0]=1;
		}
	}
	if(sys.intFlag[0])
	{
		sys.intFlag[0]=0;
		if(falconp.phase==0)
		{
			falconp.phase=1;
			falconp.phaseTime[0]=sys.intTime[0]-falconp.lasttime;
			falconp.lasttime=sys.intTime[0];
			falconp.draw|=0x10;
			sys.sensors.SensorData[1]=1;
		}
	}
	falconp.angle=falconpage_getRawAngle();
	falconp.phi=falconpage_getPhi(falconp.angle);
	sys.sensors.SensorData[2]=falconp.phi;
	sys.sensors.SensorData[3]=1000.0f/(falconp.phaseTime[1]+falconp.phaseTime[0]);
	//sys.sensors.SensorData[3]=falconp.phase;
}

void falconpage_intReset(void)
{
	sys.sensors.SensorData[0]=0;
	sys.sensors.SensorData[1]=0;	
}

float falconpage_getRawAngle(void)
{
	float t=atan2f((sys.sensors.ADCData[6]-*falconp.aat_sin_off)*(*falconp.aat_sin_scl),
			(sys.sensors.ADCData[7]-*falconp.aat_cos_off)*(*falconp.aat_cos_scl));
	if(t<0)
	{
		t+=TwoPI;
	}
	return t;
}

//a-b
float anglesub(float a,float b)
{
	if(a>b)
		return a-b;
	return a+TwoPI-b;
}

void falconpage_setPhiCalc(void)
{
	float t1,t2;
	t1=anglesub(*falconp.aat_up_ang,*falconp.aat_down_ang);
	t2=anglesub(*falconp.aat_down_ang,*falconp.aat_up_ang);
	if(t1<t2)
	{
		falconp.strokeDir=1;
		falconp.phiOffset=t1/2;
	}
	else
	{
		falconp.strokeDir=-1;
		falconp.phiOffset=t2/2;
	}
}

float falconpage_getPhi(float a)
{
	float phi;
	phi=a-*falconp.aat_down_ang;
	while(phi<0)
		phi+=TwoPI;
	while(phi>=TwoPI)
		phi-=TwoPI;
	if(phi>PI)
		phi-=TwoPI;
	//phi-=falconp.phiOffset;
	return phi*falconp.strokeDir-falconp.phiOffset;
}
