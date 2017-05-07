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




//加速度计用椭球拟合
#define IMU_ACCEL_EILLPSOIL_FIT 
#define IMU_ACCEL_EILLPOSIL_INIT

//是否初始化椭球拟合参数,如果不进行初始化,系数设为1,偏差设为0
#define IMU_ACCEL_EILLPSOIL_INIT 	
#define IMU_MAG_EILLPSOIL_INIT 




typedef	struct 
{
	/*sensor data*/
	float accel_raw[3];			//传感器 加速度,m/s^2
	float gyro_raw[3];			//传感器 陀螺仪,deg/s
	float mag_raw[3];			//传感器 磁力计,
	float baro_raw;				//传感器 气压计高度,m
	float ultra_raw;			//传感器 超声波高度,m
	/*sensor data end*/

	float accel_coff[3];
	float accel_offset[3];		//加速度零偏,m/s^2
	float gyro_offset[3];		//陀螺仪零偏,deg/s
	float mag_coff[3];
	float mag_offset[3];
	

	float accel_b[3];			//加速度(机体坐标系),m/s^2
	float accel_w[3];			//加速度(世界坐标系),m/s^2

	float mag_b[3];				//磁力计(机体坐标系),
	float mag_w[3];				//磁力计(世界坐标系),

	float velocity[3];			//速度(世界坐标系),m/s
	float position[3];			//位置(世界坐标系),m
	//float pos_gps

	float gyro[3];				//陀螺仪,(thetadot,机体坐标系),rad/s
	float gyro_deg[3];			//陀螺仪,(thetadot,机体坐标系),deg/s
	float omega[3];				//三轴角速度(欧拉方程用??),rad/s

	float orientation[3];		//姿态欧拉角,rad
	float orientation_deg[3];	//姿态欧拉角,度
	
	float height_baro;			//气压计高度,m
	float height_ultra;			//超声波高度,m

	quaternion q;				//四元数
	float dcm[3][3];				//变换矩阵(R:body->world)
	float dcm_cor[3][3];

	float gravity_b[3];			//重力在机体的分量,m/s^2
	
	float accel_norm;

	//保存以加速计算
	float sin_roll;
	float cos_roll;
	float sin_pitch;
	float cos_pitch;
	float sin_yaw;
	float cos_yaw;


	uint8_t hasIMUReady;
	
	
//	uint8_t hasAttiUpdated;		//姿态
	uint8_t hasOriUPdated;		//方向
	uint8_t hasAltUpdated;		//高度
	uint8_t hasPosUpdated;		//位置


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

