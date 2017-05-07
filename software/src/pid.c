#include "pid.h"
#include "sys.h"

#include "cmath.h"
#include "math.h"


//tau = 1/(2*pi*fc)
// Examples for _filter:
// f_cut = 10 Hz -> _filter = 15.9155e-3
// f_cut = 15 Hz -> _filter = 10.6103e-3
// f_cut = 20 Hz -> _filter =  7.9577e-3
// f_cut = 25 Hz -> _filter =  6.3662e-3
// f_cut = 30 Hz -> _filter =  5.3052e-3
#define PID_D_TERM_FILTER 15.9155e-3    // 20hz filter on D term 


void pospid_init(pid_t *pid, float Kp, float Ki, float Kd,float iMax)
{
	pid->error = 0.0f;
	pid->error_pre = 0.0f;
	pid->integ = 0.0f;
	pid->deriv = NAN;

	pid->kp = Kp;
	pid->ki = Ki;
	pid->kd = Kd;
	
	pid->iLimit = ABS(iMax);
	
	pid->lg_iMax = 0.0f;
	pid->lg_outMax = 0.0f;
}


void pospid_reset(pid_t *pid)
{
	pid->error = 0.0f;
	pid->error_pre = 0.0f;
	pid->integ = 0.0f;
	pid->deriv = NAN;
}


float pid_getP(pid_t *pid,float error)
{
	return pid->kp*error;
}

float pid_getI(pid_t *pid,float error,float dt)
{
		if((pid->ki != 0.0f)&&(dt != 0.0f))
		{
			pid->integ += error*dt;
			//»ý·ÖÏÞ·ù
			if(pid->integ < - pid->iLimit){pid->integ  = -pid->iLimit;}
			else if(pid->integ > pid->iLimit){pid->integ  = pid->iLimit;}
			
			return pid->ki*pid->integ;
		}
		return 0.0f;
}

float pid_getD(pid_t *pid,float error,float dt)
{
	if((pid->kd != 0.0f)&&(dt != 0.0f))
	{
		float derivation;
		if(isnan(pid->deriv))
		{
			derivation = 0.0f;
			pid->deriv = 0.0f;
		}
		else
		{
			derivation = (error - pid->error_pre)/dt;
		}
		pid->deriv += (dt/(PID_D_TERM_FILTER+dt))*(derivation - pid->deriv);
		pid->error_pre = error;
		return pid->kd*pid->deriv;
	}
	return 0.0f;
}

float pid_getD2(pid_t *pid,float error,float dt)
{
	if((pid->kd != 0.0f)&&(dt != 0.0f))
	{
		pid->deriv = (error - pid->error_pre)/dt;
		
		if(isnan(pid->deriv))
		{
			pid->deriv = 0.0f;
		}
		
		pid->error_pre = error;
		return pid->kd*pid->deriv;
	}
	return 0.0f;
}
float pid_getPID2(pid_t *pid,float error,float dt)
{
	pid->error = error;
	pid->output = pid_getP(pid,error)+pid_getI(pid,error,dt)+pid_getD2(pid,error,dt);
	
	pid->lg_outMax = MAX(pid->lg_outMax,ABS(pid->output));
	return pid->output;
}




float pid_getPID(pid_t *pid,float error,float dt)
{
	pid->output = pid_getP(pid,error)+pid_getI(pid,error,dt)+pid_getD(pid,error,dt);
	return pid->output;
}


float pid_getPD(pid_t *pid,float error,float dt)
{
	return pid_getP(pid,error)+pid_getD(pid,error,dt);
}



