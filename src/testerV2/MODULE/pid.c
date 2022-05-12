#include "pid.h"
#include "MathHelper.h"

void PIDInit(PIDData *pid)
{
	pid->integral=0;
	pid->lasterr=0;
	pid->out=0;
	if(pid->lpfEnabled)
		FilterClear(pid->lpf);
}

void PIDCalc(PIDData *pid,float err,float dt)
{
	float u,u0;
	if(pid->lpfEnabled)
		pid->err=FilterProcess(pid->lpf,err);
	else
		pid->err=err;
	//pid->err=pid->lpf*pid->err+(1-pid->lpf)*err;

	pid->integral=pid->integral+dt/2*(pid->err+pid->lasterr);
	pid->integral=Clampf(pid->integral,-pid->maxint,pid->maxint);
	pid->derivate=(2*pid->tau-dt)/(2*pid->tau+dt)*pid->derivate;
	pid->derivate+=(2/(2*pid->tau+dt))*(pid->err-pid->lasterr);
	
	u0=pid->P*pid->err+pid->I*pid->integral+pid->D*pid->derivate;
	u=Clampf(u0,pid->minout,pid->maxout);
	
	if(pid->I>1e-5f||pid->I<-1e-5f)
		pid->integral+=dt/pid->I*(u-u0);
	
	pid->lasterr=err;
	pid->out=u;
}
