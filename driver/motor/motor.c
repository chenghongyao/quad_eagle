#include "motor.h"
#include "sys.h"



void motor_set(uint16_t MHL,uint16_t MHR,uint16_t MTL,uint16_t MTR)
{
	MOTOR_HL = (float)(1000+MHL);
	MOTOR_HR = (float)(1000+MHR);
	MOTOR_TL = (float)(1000+MTL);
	MOTOR_TR = (float)(1000+MTR);
}


void motor_init()
{
	motor_set(0,0,0,0);
}



