#ifndef _PID_H
#define _PID_H

#include "sys.h"

typedef struct 
{
	float kp;	
	float ki;
	float kd;

	float desired;			//期望值
	float measure;			//测量值
	float error;			//误差 e(k) =desired(k)-measure(k) 
	
	
	float error_pre;		//e(k-1)
	float integ;			//积分 integ += e(k);
	float iLimit;			//积分限幅
	float deriv;			//微分项 (e(k)-e(k-1))/dt
	float output;			//输出
	
	float lg_outMax;	//输出最大
	float lg_iMax;		//积分最大 
}pid_t;


void pospid_reset(pid_t *pid);
void pospid_init(pid_t *pid, float Kp, float Ki, float Kd, float iMax);
//float pospid_next(pid_t *pid, float desired, float measure, float dt);
float pid_getPID(pid_t *pid,float error,float dt);
float pid_getPID2(pid_t *pid,float error,float dt);	//D不滤波
void pospid_init_w(pid_t *pid, float Kp, float Ki, float Kd,float iMax,float eMax);

#endif
