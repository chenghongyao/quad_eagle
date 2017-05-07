#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "sys.h"
#include "pid.h"



#define CTRL_MOTOR_NUM  4
#define CTRL_THRO_LAND	100
//#define CTRL_MOTOR_MAX	1000.0f

#define CTRL_THRO_HOLDALT_BASE	450

/*�����޷�*/
#define ATTI_RATE_INT_LIMIT	1000.0f		//��̬�ڻ������޷�
#define ANGLE_INT_LIMIT 		1000.0f		//��̬�⻷�����޷�

#define ALT_RATE_INT_LIMIT	1000.0f		//�߶��ڻ������޷�
#define ALT_INT_LIMIT 			1000.0f		//�߶��⻷�����޷�

/*����޷�*/
#define CTRL_ATTI_RATE_EXP_MAX 			100.0				//��̬�⻷����޷�,�����ת�ٶ�
#define CTRL_ATTI_RATE_PR_OUT_MAX 	350.0f			//��̬�ڻ�����޷�
#define CTRL_ATTI_RATE_YAW_OUT_MAX  350.0f			//

#define CTRL_ALT_RATE_EXP_MAX 1.0f					//�߶��⻷����޷�,��������ٶ�
#define CTRL_ALT_RATE_OUT_MAX	300.0f				//�߶��ڻ�����޷�,��󸽼�����

typedef	struct 
{
	//�����������
	float thro;
	float roll;
	float pitch;
	float yaw;			
	float power[CTRL_MOTOR_NUM];	
	uint16_t motor[CTRL_MOTOR_NUM];	//���ռ�ձ�
  
	float atti_rate_target[3];					//��������,��
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


extern pid_t pidPitchAngle, pidRollAngle, pidYawAngle;		//��̬�⻷pid
extern pid_t pidPitchRate, pidRollRate, pidYawRate;				//��̬�ڻ�
extern pid_t pidAlt, pidAltRate;													//�߶�,z������
extern pid_t pidPosX, pidPosY;														//λ���⻷pid
extern pid_t pidPosXRate, pidPosYRate;										//λ���ڻ�pid

extern controller_t mController;


void controller_init(void);
void controller_dealAttiRate(void);
void controller_dealAtti(void);
void controller_dealAltRate(void);
void controller_dealAlt(void);
void controller_setMotors(void);
void power2motor(float *power,uint16_t *motor);

#endif
