#ifndef MPU6050_H
#define MPU6050_H
#include "sys.h"



#define CONSTANTS_ONE_G					9.80665f	
//���ݳ�ʼ��ֵ�Ը����̽����޸�
#define MPU6050_MAXG	8.0f				//�������� 
#define MPU6050_MAXW	2000.0f			//���ٶ�����

//���ٶ�����ٶ�ADC����ֵ,��ʵֵΪADCֵ���Ըñ���ֵ
#define MPU_K_GYRO	(MPU6050_MAXW/32768.0f)
#define MPU_K_ACCEL	(MPU6050_MAXG*CONSTANTS_ONE_G/32768.0f)
//=======================================================================

void mpu6050_init(void);			//MPU��ʼ��
void mpu6050_getGyroAdc(int16_t *gyroadc);
void mpu6050_getAccAdc(int16_t *accadc);
void mpu6050_getAccel(float *acc);
void mpu6050_getGyro(float *gyro);
#endif


