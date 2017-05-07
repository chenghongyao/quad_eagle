#ifndef _PID_H
#define _PID_H

#include "sys.h"

typedef struct 
{
	float kp;	
	float ki;
	float kd;

	float desired;			//����ֵ
	float measure;			//����ֵ
	float error;			//��� e(k) =desired(k)-measure(k) 
	
	
	float error_pre;		//e(k-1)
	float integ;			//���� integ += e(k);
	float iLimit;			//�����޷�
	float deriv;			//΢���� (e(k)-e(k-1))/dt
	float output;			//���
	
	float lg_outMax;	//������
	float lg_iMax;		//������� 
}pid_t;


void pospid_reset(pid_t *pid);
void pospid_init(pid_t *pid, float Kp, float Ki, float Kd, float iMax);
//float pospid_next(pid_t *pid, float desired, float measure, float dt);
float pid_getPID(pid_t *pid,float error,float dt);
float pid_getPID2(pid_t *pid,float error,float dt);	//D���˲�
void pospid_init_w(pid_t *pid, float Kp, float Ki, float Kd,float iMax,float eMax);

#endif
