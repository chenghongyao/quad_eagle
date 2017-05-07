#ifndef _IMU_H
#define _IMU_H
#include "sys.h"
#include "quat.h"



/*LPF accel gyro*/
#define IMU_SAMPLE_RATE 						200.0f
#define IMU_FILTER_CUTOFF_FREQ			20.0f		


#define IMU_DEVICE_NONE					0
#define IMU_DEVICE_MS5611				1
#define IMU_DEVICE_ULTASOUND		2

//#define IMU_USE_HMC5883L
#define IMU_ALT_DEVICE	IMU_DEVICE_NONE




//���ٶȼ����������
#define IMU_ACCEL_EILLPSOIL_FIT 
#define IMU_ACCEL_EILLPOSIL_INIT

//�Ƿ��ʼ��������ϲ���,��������г�ʼ��,ϵ����Ϊ1,ƫ����Ϊ0
#define IMU_ACCEL_EILLPSOIL_INIT 	
#define IMU_MAG_EILLPSOIL_INIT 




typedef	struct 
{
	/*sensor data*/
	float accel_raw[3];			//������ ���ٶ�,m/s^2
	float gyro_raw[3];			//������ ������,deg/s
	float mag_raw[3];			//������ ������,
	float baro_raw;				//������ ��ѹ�Ƹ߶�,m
	float ultra_raw;			//������ �������߶�,m
	/*sensor data end*/

	float accel_coff[3];
	float accel_offset[3];		//���ٶ���ƫ,m/s^2
	float gyro_offset[3];		//��������ƫ,deg/s
	float mag_coff[3];
	float mag_offset[3];
	

	float accel_b[3];			//���ٶ�(��������ϵ),m/s^2
	float accel_w[3];			//���ٶ�(��������ϵ),m/s^2

	float mag_b[3];				//������(��������ϵ),
	float mag_w[3];				//������(��������ϵ),

	float velocity[3];			//�ٶ�(��������ϵ),m/s
	float position[3];			//λ��(��������ϵ),m
	//float pos_gps

	float gyro[3];				//������,(thetadot,��������ϵ),rad/s
	float gyro_deg[3];			//������,(thetadot,��������ϵ),deg/s
	float omega[3];				//������ٶ�(ŷ��������??),rad/s

	float orientation[3];		//��̬ŷ����,rad
	float orientation_deg[3];	//��̬ŷ����,��
	
	float height_baro;			//��ѹ�Ƹ߶�,m
	float height_ultra;			//�������߶�,m

	quaternion q;				//��Ԫ��
	float dcm[3][3];				//�任����(R:body->world)
	float dcm_cor[3][3];

	float gravity_b[3];			//�����ڻ���ķ���,m/s^2
	
	float accel_norm;

	//�����Լ��ټ���
	float sin_roll;
	float cos_roll;
	float sin_pitch;
	float cos_pitch;
	float sin_yaw;
	float cos_yaw;


	uint8_t hasIMUReady;
	
	
//	uint8_t hasAttiUpdated;		//��̬
	uint8_t hasOriUPdated;		//����
	uint8_t hasAltUpdated;		//�߶�
	uint8_t hasPosUpdated;		//λ��


}imu_t;


extern imu_t mImu;

void imu_init(void);
void imu_updateSensor(void);
void imu_update(void);
void imu_getGravity(float g[3], float R[3][3]);

void newton(float *x,float *v,float a,float dt);


void quaternion2dcm(quaternion *q, float R[3][3]);
void dcm2angle(float R[3][3], float angel[3]);
void body2world(float R[3][3], float body[3], float world[3]);
void world2body(float R[3][3], float body[3], float world[3]);

float imu_getMagYaw(float mx,float my);


#endif

