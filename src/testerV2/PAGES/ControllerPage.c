#include "pages.h"
#include "oled.h"
#include "pwm.h"
#include "math.h"
#include "MathHelper.h"
#include "pid.h"
#include "filter.h"

#define Controller_Main 0x01
#define Controller_Main_Freq 0x11
#define Controller_Main_Ratio 0x21
#define Controller_Tune 0x02
#define Controller_Tune_P 0x12
#define Controller_Tune_I 0x22
#define Controller_Tune_D 0x42



#define Upstroke 0
#define Downstroke 1

void controlpage_showData(u8 f);
void controlpage_fastUpdate(u16 time);
void controlpage_intUpdate(void);
void controlpage_intReset(void);

float controlpage_getRawAngle(void);
void controlpage_setPhiCalc(void);
float controlpage_getPhi(float a);
void controlpage_initController(void);

struct
{
	PIDData pid;
	Filter lpf;
	const s32* pwm_min;
	const s32* pwm_max;
	const s32* pwm_disarmed;
	float* aat_sin_scl;
	s32* aat_sin_off;
	float* aat_cos_scl;
	s32* aat_cos_off;
	float* aat_up_ang;
	float* aat_down_ang;
	float* stk_max_phi;
	
	float* ctrl_p;
	float* ctrl_i;
	float* ctrl_d;
	
	u32 caliCnt;
	s32 smax,smin,cmax,cmin;
	float angle;
	float phi;
	float phiOffset;
	
	float freqtgt;
	float ratiotgt;
	float maxphi;
	float minphi;
	float gain;
	float asinOffset;
	float asinDir;
	float theta;
	float theta_d;
	float theta_d_real;
	float omega;
	float omega_d;
	u8 ctrlPhase;
	u8 phiBdryRdy;
	
	u8 state;
	u8 substate;
	//u8 currch;
	//u8 bind;
	u8 phase;
	u32 lasttime;
	s32 phaseTime[2];
	u16 pwm[2];
	u8 draw;
	s8 strokeDir;
	u8 drawSkip;
	
} controlp;


void PageInit_controller(u8 f)
{
	if(f)
	{
		controlp.state=Controller_Main;		
		controlp.phase=0;
		controlp.lasttime=0;
		controlp.phaseTime[0]=0;
		controlp.phaseTime[1]=0;
		controlp.pwm_min=ParamGetFromName("PWM_MIN");
		controlp.pwm_max=ParamGetFromName("PWM_MAX");		
		controlp.pwm_disarmed=ParamGetFromName("PWM_DISARMED");
		controlp.aat_sin_scl=ParamGetFromName("AAT_SIN_SCL");
		controlp.aat_sin_off=ParamGetFromName("AAT_SIN_OFF");
		controlp.aat_cos_scl=ParamGetFromName("AAT_COS_SCL");
		controlp.aat_cos_off=ParamGetFromName("AAT_COS_OFF");
		controlp.aat_up_ang=ParamGetFromName("AAT_UP_ANG");
		controlp.aat_down_ang=ParamGetFromName("AAT_DOWN_ANG");
		controlp.stk_max_phi=ParamGetFromName("STK_PHI_MAX");
		
		controlp.ctrl_p=ParamGetFromName("STK_CTRL_P");
		controlp.ctrl_i=ParamGetFromName("STK_CTRL_I");
		controlp.ctrl_d=ParamGetFromName("STK_CTRL_D");
		
		controlpage_setPhiCalc();
		
		controlp.pwm[0]=*controlp.pwm_disarmed;
		controlp.pwm[1]=*controlp.pwm_disarmed;
		
		controlp.freqtgt=0;
		controlp.ratiotgt=0.5;
		controlp.maxphi=controlpage_getPhi(*controlp.aat_up_ang);
		controlp.minphi=controlpage_getPhi(*controlp.aat_down_ang);
		controlp.phiBdryRdy=0;
		controlp.gain=1.0;		
		
		//3rd-order butterworth,fs=500,fc=12
//		controlp.lpf.order=3;
//		controlp.lpf.num[0]=0.00037068337;
//		controlp.lpf.num[1]=0.00111205012;
//		controlp.lpf.num[2]=0.00111205012;
//		controlp.lpf.num[3]=0.00037068337;
//		controlp.lpf.den[0]=1;
//		controlp.lpf.den[1]=-2.6986876695;
//		controlp.lpf.den[2]=2.44107970854;
//		controlp.lpf.den[3]=-0.7394265720;

//		//3rd-order butterworth,fs=1000,fc=50
//		controlp.lpf.order=3;
//		controlp.lpf.num[0]=0.002898194633721f;
//		controlp.lpf.num[1]=0.008694583901164f;
//		controlp.lpf.num[2]=0.008694583901164f;
//		controlp.lpf.num[3]=0.002898194633721f;
//		controlp.lpf.den[0]=1.0f;
//		controlp.lpf.den[1]=-2.374094743709352f;
//		controlp.lpf.den[2]=1.929355669091216f;
//		controlp.lpf.den[3]=-0.532075368312092f;
		
		//3rd-order butterworth,fs=1000,fc=15
		controlp.lpf.order=3;
		controlp.lpf.num[0]=9.544250842372823e-05;
		controlp.lpf.num[1]=2.863275252711847e-04;
		controlp.lpf.num[2]=2.863275252711847e-04;
		controlp.lpf.num[3]=9.544250842372823e-05;
		controlp.lpf.den[0]=1.0f;
		controlp.lpf.den[1]=-2.811573677324689f;
		controlp.lpf.den[2]=2.640483492778340f;
		controlp.lpf.den[3]=-0.828146275386261f;
		
		controlp.pid.minout=*(float*)ParamGetFromName("STK_THRO_MIN");
		controlp.pid.maxout=*(float*)ParamGetFromName("STK_THRO_MAX");
		controlp.pid.P=*controlp.ctrl_p;
		controlp.pid.I=*controlp.ctrl_i;
		controlp.pid.D=*controlp.ctrl_d;
		controlp.pid.tau=1.0/500;
		controlp.pid.maxint=*(float*)ParamGetFromName("STK_INT_MAX");
		controlp.pid.lpfEnabled=0;
		
		controlpage_initController();
		return;
	}
	
	sys.slowUpdate=0;
	sys.fastUpdate=controlpage_fastUpdate;
	sys.intUpdate=controlpage_intUpdate;
	sys.intReset=controlpage_intReset;	
	
	OledClearBuff();
	OledClear(0);
	
	PagesDrawHeader(ControllerPage,"StrokeCtrl");
	
	if(PWMIsArmed())
	{
		OledDispString(0,2,"ARMED",0);
	}
	
	OledDispString(0,4,"TGT :     Hz|     %",0);
	OledDispString(0,5,"SENS:     Hz|     %",0);
	OledDispString(0,6,"PHA :     Hz|     %",0);
	OledDispString(2,8,"P:00.000  I:00.000",0);
	OledDispString(2,9,"D:00.000",0);
//	OledDispString(0,7,"PWM  :2000ms|     %",0);
//	OledDispString(6,8,"    ms|     %",0);	
	OledDispString(0,11,"OUT :       |     %",0);
	
	OledDispString(0,13,"Phi         |",0);	
	
	OledDispString(17,15,"TUNE",0);
	
	controlpage_showData(0xFF);
	controlp.drawSkip=0;	
}

void controlpage_initController(void)
{		
	controlp.asinDir=1;
	controlp.asinOffset=0;
	controlp.ctrlPhase=0;
	controlp.theta=0;
	controlp.theta_d=0;
	controlp.theta_d_real=0;
	controlp.omega=0;
	controlp.omega_d=0;
	
	FilterClear(&controlp.lpf);		
}

void control_update_main(void)
{
	u8 i;
	s16 dpwm=0;
	float dfreq;
	//normal state
	controlp.draw|=0x20;
	controlp.draw&=0xf0;
	
	if(PWMIsArmed())
	{
		if(wheelPress)
		{
			PWMDisarm();
			controlp.pwm[0]=*controlp.pwm_disarmed;
			controlp.pwm[1]=*controlp.pwm_disarmed;
			controlp.draw|=0x03;
			OledDispString(0,2,"      ",0);
			OledDispString(17,15,"TUNE",0);
			controlp.theta_d=0;
			controlp.freqtgt=0;
			controlp.state=Controller_Main;
			controlp.draw|=0x01;
		}
		if(keyPress&(KEY_RIGHT|KEY_LEFT))
		{
			if(controlp.state==Controller_Main_Freq)
				controlp.state=Controller_Main_Ratio;
			else
				controlp.state=Controller_Main_Freq;
			controlp.draw|=0x01;
		}
		dfreq=0;
		if(keyPress&KEY_UP)
		{
			dfreq=10;
		}
		if(keyPress&KEY_DOWN)
		{
			dfreq=-10;
		}
		if(currWheel>lastWheel)
		{
			dfreq=1;
		}
		if(currWheel<lastWheel)
		{
			dfreq=-1;
		}
		if(dfreq!=0)
		{
			if(controlp.state==Controller_Main_Freq)
			{
				controlp.freqtgt+=dfreq/10;
				if(controlp.freqtgt>0.05)
				{
					controlp.freqtgt=Clampf(controlp.freqtgt,1,5);
				}
				else
				{
					controlp.freqtgt=0;
				}
			}
			else
			{
				controlp.ratiotgt+=dfreq/100;
				controlp.ratiotgt=Clampf(controlp.ratiotgt,0.1f,0.9f);
			}
			controlp.draw|=0x01;
		}		
//		for(i=0;i<2;i++)
//		{
//			if(controlp.pwm[i]<*controlp.pwm_min)
//				controlp.pwm[i]=*controlp.pwm_min;
//			if(controlp.pwm[i]>*controlp.pwm_max)
//				controlp.pwm[i]=*controlp.pwm_max;
//		}
//		sys.pwm[0]=controlp.pwm[controlp.phase];
//		sys.pwm[1]=controlp.pwm[controlp.phase];
//		PWMSet(sys.pwm[0],sys.pwm[1]);		
	}
	else
	{
		if(wheelPress)
		{
			PIDInit(&controlp.pid);
			
			PWMArm();
			controlp.theta_d=0;
			controlp.theta=0;
			controlp.asinOffset=0;
			
			controlp.pwm[0]=*controlp.pwm_min;
			controlp.pwm[1]=*controlp.pwm_min;
			controlp.draw|=0x07;
			OledDispString(0,2,"ARMED",0);
			OledDispString(17,15,"    ",0);
			controlp.state=Controller_Main_Freq;
			
		}
		
		if(keyPress&KEY_LEFT)
		{
			PagesNext(-1);
			controlp.draw=0;
		}
		if(keyPress&KEY_RIGHT)
		{
			PagesNext(1);
			controlp.draw=0;
		}
		if(keyPress&KEY_C)
		{
			OledDispString(17,15,"SAVE",0);
			controlp.state=Controller_Tune_P;
			controlp.draw|=0x04;
		}
		
	}
}

void control_update_tune(void)
{
	float dpid;
	if(keyPress&KEY_C)
	{
		OledDispString(17,15,"TUNE",0);
		controlp.state=Controller_Main;
		*controlp.ctrl_p=controlp.pid.P;
		*controlp.ctrl_i=controlp.pid.I;
		*controlp.ctrl_d=controlp.pid.D;
		PIDInit(&controlp.pid);
		ParamWrite();
		controlp.draw|=0x07;
	}
	if(keyPress&KEY_LEFT)
	{
		if(controlp.state==Controller_Tune_P)
			controlp.state=Controller_Tune_D;
		else if(controlp.state==Controller_Tune_I)
			controlp.state=Controller_Tune_P;
		else if(controlp.state==Controller_Tune_D)
			controlp.state=Controller_Tune_I;
		controlp.draw|=0x04;
	}
	if(keyPress&KEY_RIGHT)
	{
		if(controlp.state==Controller_Tune_P)
			controlp.state=Controller_Tune_I;
		else if(controlp.state==Controller_Tune_I)
			controlp.state=Controller_Tune_D;
		else if(controlp.state==Controller_Tune_D)
			controlp.state=Controller_Tune_P;
		controlp.draw|=0x04;
	}
	dpid=0;
	if(keyPress&KEY_UP)
	{
		dpid=0.1f;
	}
	if(keyPress&KEY_DOWN)
	{
		dpid=-0.1f;
	}
	if(currWheel>lastWheel)
	{
		dpid=0.01f;
	}
	if(currWheel<lastWheel)
	{
		dpid=-0.01f;
	}
	if(dpid!=0)
	{
		if(controlp.state==Controller_Tune_P)
			controlp.pid.P+=dpid;
		else if(controlp.state==Controller_Tune_I)
			controlp.pid.I+=dpid;
		else if(controlp.state==Controller_Tune_D)
			controlp.pid.D+=dpid;
		controlp.draw|=0x04;
	}
}

void PageUpdate_controller(void)
{	
	controlp.drawSkip++;
	if(controlp.state&Controller_Main)
	{
		control_update_main();		
	}
	else if(controlp.state&Controller_Tune)
	{
		control_update_tune();
	}
	controlpage_showData(controlp.draw);
	
}

void controlpage_showData(u8 f)
{
	//OledDispInt(0,8,currWheel,5,0);
	//OledDispInt(0,9,currKey,3,0);
	
	if(f&0x01)
	{
		if(controlp.state==Controller_Main_Freq)			
			OledDispFixed(5,4,roundf(controlp.freqtgt*100),2,5,1);
		else
			OledDispFixed(5,4,roundf(controlp.freqtgt*100),2,5,0);
		if(controlp.state==Controller_Main_Ratio)
			OledDispFixed(13,4,roundf(controlp.ratiotgt*10000),2,5,1);
		else
			OledDispFixed(13,4,roundf(controlp.ratiotgt*10000),2,5,0);
	}
	if(f&0x02)
	{		
		
	}
	if(f&0x04)
	{
		if(controlp.state==Controller_Tune_P)
			OledDispFixed(4,8,roundf(controlp.pid.P*1000),3,6,1);
		else
			OledDispFixed(4,8,roundf(controlp.pid.P*1000),3,6,0);
		
		if(controlp.state==Controller_Tune_I)
			OledDispFixed(14,8,roundf(controlp.pid.I*1000),3,6,1);
		else
			OledDispFixed(14,8,roundf(controlp.pid.I*1000),3,6,0);
		
		if(controlp.state==Controller_Tune_D)
			OledDispFixed(4,9,roundf(controlp.pid.D*1000),3,6,1);
		else
			OledDispFixed(4,9,roundf(controlp.pid.D*1000),3,6,0);
//		OledDispInt(6,7,controlp.pwm[1],4,0);
//		OledDispInt(13,7,(controlp.pwm[1]-1000)/10,4,0);
//		OledDispInt(6,10,sys.pwm[0],4,0);
//		OledDispInt(13,10,(sys.pwm[0]-1000)/10,4,0);
	}
	if(f&0x10)
	{		
		OledDispFixed(5,5,100000/(controlp.phaseTime[0]+controlp.phaseTime[1]),2,5,0);
		OledDispFixed(13,5,controlp.phaseTime[0]*10000/(controlp.phaseTime[0]+controlp.phaseTime[1]),2,5,0);
		f&=0xEF;
	}	
	if(f&0x20)
	{
		OledDispFixed(4,13,controlp.angle*5730,2,8,0);
		OledDispFixed(13,13,controlp.phi*5730,2,8,0);
		f&=0xDF;
	}
	if(f&0x40)
	{
		OledDispInt(5,11,controlp.pwm[0],4,0);
		OledDispInt(13,11,(controlp.pwm[0]-1000)/10,4,0);
		f&=0xBF;
	}
	
}
void controlpage_fastUpdate(u16 time)
{
	float dt=time/1000.0f;
	
	if(!PWMIsArmed())
		return;
	
	
	//update phase
	if(controlp.ctrlPhase==Upstroke)
	{
		if(controlp.phase==Downstroke)
		{
			controlp.ctrlPhase=Downstroke;
			controlp.asinDir=-controlp.asinDir;
			controlp.asinOffset+=PI;			
		}
	}
	else
	{
		if(controlp.phiBdryRdy==0x03&&controlp.phase==Upstroke)
		{
			if(controlp.lpf.val<0.97f*controlp.minphi)
			{
				controlp.ctrlPhase=Upstroke;
				controlp.asinDir=-controlp.asinDir;
				controlp.asinOffset+=PI;	
				
			}
		}
	}
	//update theta & omega
	float range;
	float mid;
	float theta;
	float deadband;
	u8 flag;
	
//	range=(controlp.maxphi-controlp.minphi)/2;
	range=*controlp.stk_max_phi;
	mid=(controlp.maxphi+controlp.minphi)/2;
	theta=(controlp.lpf.val-mid)/range;
//	deadband=(controlp.maxphi-controlp.minphi)/2;
	flag=0;
	if(controlp.lpf.val>controlp.maxphi*0.95||controlp.lpf.val<controlp.minphi*0.95)
	{
		flag=1;
	}
	
	theta=Clampf(theta,-1.0f,1.0f);
	theta=asinf(theta)*controlp.asinDir+controlp.asinOffset;	
	
	//generate theta_d
	controlp.omega_d=controlp.freqtgt*TwoPI;
	controlp.theta_d+=controlp.omega_d*dt;
//	controlp.theta_d_real+=controlp.omega_d*dt;
//	if(controlp.theta_d_real>TwoPI)
//		controlp.theta_d_real-=TwoPI;
	
	if(controlp.asinOffset>3*TwoPI)
	{
		controlp.asinOffset-=3*TwoPI;
		controlp.theta_d-=3*TwoPI;
	}
	
	float volt=0;
	if(flag)
	{
		//bypass
		controlp.theta+=controlp.omega*dt;
	}
	else
	{
		controlp.omega=(theta-controlp.theta)/dt;
		controlp.theta=theta;		
	}
	//pid control
	PIDCalc(&controlp.pid,(controlp.theta_d-controlp.theta)/TwoPI,dt);
	volt=controlp.pid.out*controlp.pid.out;
	controlp.pwm[0]=volt*1000+1000;
	sys.pwm[0]=controlp.pwm[0];
	sys.pwm[1]=controlp.pwm[0];
	sys.pwm[0]=1050;
	sys.pwm[1]=1050;
	PWMSet(sys.pwm[0],sys.pwm[1]);
	
	controlp.draw|=0x40;
	//printf("%d\t%f\t%f\t%f\r\n",time,controlp.theta_d,controlp.theta,volt);
	sys.sensors.SensorData[0]=controlp.phi;
	
	sys.sensors.SensorData[1]=controlp.theta;
	sys.sensors.SensorData[2]=controlp.theta_d;
	sys.sensors.SensorData[3]=controlp.omega;
	sys.sensors.SensorData[2]=flag;
	//sys.sensors.SensorData[1]=controlp.angle;
}

void controlpage_intUpdate(void)
{
	//almost max downstroke pos
	if(sys.intFlag[1])
	{
		sys.intFlag[1]=0;
		if(controlp.phase==Downstroke)
		{
			controlp.phase=Upstroke;
			controlp.phaseTime[1]=sys.intTime[1]-controlp.lasttime;
			controlp.lasttime=sys.intTime[1];
			controlp.draw|=0x10;
			//sys.sensors.SensorData[0]=1;
			controlp.phiBdryRdy|=0x01;
		}
	}
	//max upstroke pos
	if(sys.intFlag[0])
	{
		sys.intFlag[0]=0;
		if(controlp.phase==Upstroke)
		{
			controlp.phase=Downstroke;
			controlp.phaseTime[0]=sys.intTime[0]-controlp.lasttime;
			controlp.lasttime=sys.intTime[0];
			controlp.draw|=0x10;
			//sys.sensors.SensorData[1]=1;
			controlp.phiBdryRdy|=0x02;			
		}
	}
	controlp.angle=controlpage_getRawAngle();
	controlp.phi=controlpage_getPhi(controlp.angle);
	FilterProcess(&controlp.lpf,controlp.phi);
	//sys.sensors.SensorData[2]=controlp.phi;
	//sys.sensors.SensorData[3]=1000.0f/(controlp.phaseTime[1]+controlp.phaseTime[0]);
	//sys.sensors.SensorData[2]=controlp.lpf.val;
	if(controlp.lpf.val>controlp.maxphi)
		controlp.maxphi=controlp.lpf.val;
	if(controlp.lpf.val<controlp.minphi)
		controlp.minphi=controlp.lpf.val;
	
}

void controlpage_intReset(void)
{
//	sys.sensors.SensorData[0]=0;
//	sys.sensors.SensorData[1]=0;	
}

float controlpage_getRawAngle(void)
{
	float t=atan2f((sys.sensors.ADCData[6]-*controlp.aat_sin_off)*(*controlp.aat_sin_scl),
			(sys.sensors.ADCData[7]-*controlp.aat_cos_off)*(*controlp.aat_cos_scl));
	if(t<0)
	{
		t+=TwoPI;
	}
	return t;
}


void controlpage_setPhiCalc(void)
{
	float t1,t2;
	t1=AngleSub(*controlp.aat_up_ang,*controlp.aat_down_ang);
	t2=AngleSub(*controlp.aat_down_ang,*controlp.aat_up_ang);
	if(t1<t2)
	{
		controlp.strokeDir=1;
		controlp.phiOffset=t1/2;
	}
	else
	{
		controlp.strokeDir=-1;
		controlp.phiOffset=t2/2;
	}
}

float controlpage_getPhi(float a)
{
	float phi;
	phi=a-*controlp.aat_down_ang;
	while(phi<0)
		phi+=TwoPI;
	while(phi>=TwoPI)
		phi-=TwoPI;
	if(phi>PI)
		phi-=TwoPI;
	
	return phi*controlp.strokeDir-controlp.phiOffset;
}
