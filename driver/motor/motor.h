#ifndef _MOTOR_H
#define _MOTOR_H
#include "sys.h"


//#define MOT_PERIOC_DUTY	20000				//�������ڼ���ֵ
//#define MOT_TO_PWMDUTY		((float)MOT_PERIOC_DUTY/20000.0f)			//20000us��PWM����ֵ/20000us

void motor_set(uint16_t MHL,uint16_t MHR,uint16_t MTL,uint16_t MTR);
void motor_init(void);
#endif
