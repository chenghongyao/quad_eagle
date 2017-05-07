#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "sys.h"
#include "pid.h"



#define CTRL_MOTOR_NUM  4
#define CTRL_THRO_LAND	100
//#define CTRL_MOTOR_MAX	1000.0f

#define CTRL_THRO_HOLDALT_BASE	450

/*积分限幅*/
#define ATTI_RATE_INT_LIMIT	1000.0f		//姿态内环积分限幅
#define ANGLE_INT_LIMIT 		1000.0f		//姿态外环积分限幅

#define ALT_RATE_INT_LIMIT	1000.0f		//高度内环积分限幅
#define ALT_INT_LIMIT 			1000.0f		//高度外环积分限幅

/*输出限幅*/
#define CTRL_ATTI_RATE_EXP_MAX 			100.0				//姿态外环输出限幅,最大旋转速度
#define CTRL_ATTI_RATE_PR_OUT_MAX 	350.0f			//姿态内环输出限幅
#define CTRL_ATTI_RATE_YAW_OUT_MAX  350.0f			//

#define CTRL_ALT_RATE_EXP_MAX 1.0f					//高度外环输出限幅,最大上升速度
#define CTRL_ALT_RATE_OUT_MAX	300.0f				//高度内化输出限幅,最大附加油门

typedef	struct 
{
	//各方向控制量
	float thro;
	float roll;
	float pitch;
	float yaw;			
	float power[CTRL_MOTOR_NUM];	
	uint16_t motor[CTRL_MOTOR_NUM];	//电机占空比
  
	float atti_rate_target[3];					//速率期望,度
	float angle_target[3];
	
	float alt_rate_target;
	float alt_target;
	float alt_thro;
}controller_t;


enum
{
	MO_HL =	0,
	MO_HR = 1,
	MO_TL = 2,
	MO_TR  = 3,
};


extern pid_t pidPitchAngle, pidRollAngle, pidYawAngle;		//姿态外环pid
extern pid_t pidPitchRate, pidRollRate, pidYawRate;				//姿态内环
extern pid_t pidAlt, pidAltRate;													//高度,z轴速率
extern pid_t pidPosX, pidPosY;														//位置外环pid
extern pid_t pidPosXRate, pidPosYRate;										//位置内环pid

extern controller_t mController;


void controller_init(void);
void controller_dealAttiRate(void);
void controller_dealAtti(void);
void controller_dealAltRate(void);
void controller_dealAlt(void);
void controller_setMotors(void);
void power2motor(float *power,uint16_t *motor);

#endif
