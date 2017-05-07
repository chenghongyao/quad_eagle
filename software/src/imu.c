#include "imu.h"
#include "sys.h"

#include "math.h"
#include "cmath.h"
#include "filter.h"
#include "ekf.h"
#include "ano.h"

imu_t mImu;
static lpf2_t lpf_ax, lpf_ay, lpf_az;
static lpf2_t lpf_gx, lpf_gy, lpf_gz;

void imu_init()
{
	//�˲�����ʼ��
	lpf_butter(&lpf_ax, IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);
	lpf_butter(&lpf_ay, IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);
	lpf_butter(&lpf_az, IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);
	
	lpf_butter(&lpf_gx, IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);
	lpf_butter(&lpf_gy, IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);
	lpf_butter(&lpf_gz, IMU_SAMPLE_RATE, IMU_FILTER_CUTOFF_FREQ);

	mImu.velocity[0] = 0.0f;
	mImu.velocity[1] = 0.0f;
	mImu.velocity[2] = 0.0f;

	mImu.position[0] = 0.0f;
	mImu.position[1] = 0.0f;
	mImu.position[2] = 0.0f;

	mImu.gyro_offset[0] = 0.0f;
	mImu.gyro_offset[1] = 0.0f;
	mImu.gyro_offset[2] = 0.0f;

#ifdef IMU_ACCEL_EILLPSOIL_FIT
	#ifdef IMU_ACCEL_EILLPOSIL_INIT

		mImu.accel_offset[0] = 0.098833f;
		mImu.accel_offset[1] = -0.2459f;
		mImu.accel_offset[2] = -1.9726f;
		mImu.accel_coff[0] = 1.005f;
		mImu.accel_coff[1] = 0.99375f;
		mImu.accel_coff[2] = 0.99442f;
	#else 
		mImu.accel_offset[0] = 0.0f;
		mImu.accel_offset[1] = 0.0f;
		mImu.accel_offset[2] = 0.0f;
		mImu.accel_coff[0] = 1.0f;
		mImu.accel_coff[1] = 1.0f;
		mImu.accel_coff[2] = 1.0f;
	#endif
#else
	mImu.accel_offset[0] = 0.0f;
	mImu.accel_offset[1] = 0.0f;
	mImu.accel_offset[2] = 0.0f;
	
#endif
	mImu.hasIMUReady = 0;
}



void imu_updateSensor()
{
	float temp;
	/*��ȡ������*/
	mpu6050_getAccel(mImu.accel_raw);
	mpu6050_getGyro(mImu.gyro_raw);

	/*Ԥ����,�����޸�,xǰ,y��,z��,������̧ͷ(gyro_y),�ҹ�(gyro_x),��תΪ��(gyro_z)*/
	temp = mImu.accel_raw[0];
	mImu.accel_raw[0] = -mImu.accel_raw[1];
	mImu.accel_raw[1] = -temp;
	temp = mImu.gyro_raw[0];
	mImu.gyro_raw[0] = mImu.gyro_raw[1];
	mImu.gyro_raw[1] = temp;
	mImu.gyro_raw[2] = -mImu.gyro_raw[2];
	
	
	/*���ٶȼƵ�ͨ�˲�*/
	mImu.accel_b[0] = lpf2nd_next(&lpf_ax, mImu.accel_raw[0]);
	mImu.accel_b[1] = lpf2nd_next(&lpf_ay, mImu.accel_raw[1]);
	mImu.accel_b[2] = lpf2nd_next(&lpf_az, mImu.accel_raw[2]);
	/*�����ǵ�ͨ�˲�*/
	mImu.gyro_deg[0] = lpf2nd_next(&lpf_gx, mImu.gyro_raw[0]);
	mImu.gyro_deg[1] = lpf2nd_next(&lpf_gy, mImu.gyro_raw[1]);
	mImu.gyro_deg[2] = lpf2nd_next(&lpf_gz, mImu.gyro_raw[2]);
	
#ifdef IMU_ACCEL_EILLPSOIL_FIT 														/*���ٶȼ� �������*/
	mImu.accel_b[0] = mImu.accel_coff[0]*(mImu.accel_b[0]- mImu.accel_offset[0]);
	mImu.accel_b[1] = mImu.accel_coff[1]*(mImu.accel_b[1]- mImu.accel_offset[1]);
	mImu.accel_b[2] = mImu.accel_coff[2]*(mImu.accel_b[2]- mImu.accel_offset[2]);

	{			//������ƽ������
		float xtemp = mImu.accel_b[0];
		float ytemp = mImu.accel_b[1];
		float ztemp = mImu.accel_b[2];
		
		mImu.accel_b[0] += ytemp*mTab.roll_corr*mTab.pitch_corr+ztemp*mTab.pitch_corr;
		mImu.accel_b[1] -= ztemp*mTab.roll_corr;
		mImu.accel_b[2] += ytemp*mTab.roll_corr-xtemp*mTab.pitch_corr;
	}
	
	
#else	/*���ٶȼƳ���ƫ*/
	mImu.accel_b[0] -= mImu.accel_offset[0];
	mImu.accel_b[1] -= mImu.accel_offset[1];
	mImu.accel_b[2] -= mImu.accel_offset[2];
#endif
	
	/*�����ǳ���ƫ*/
	mImu.gyro_deg[0] -= mImu.gyro_offset[0];
	mImu.gyro_deg[1] -= mImu.gyro_offset[1];
	mImu.gyro_deg[2] -= mImu.gyro_offset[2];
	deg2rad(mImu.gyro, mImu.gyro_deg,3);					//תΪ����
	
	//debug("ax=%.2f,ay=%.2f,az=%.2f,gx=%.2f,gy=%.2f,gz=%.2f\r\n", mImu.accel_raw[0], mImu.accel_raw[1], mImu.accel_raw[2], mImu.gyro_raw[0], mImu.gyro_raw[1], mImu.gyro_raw[2]);
	//debug("ax=%.2f,ay=%.2f,az=%.2f,gx=%.2f,gy=%.2f,gz=%.2f\r\n", mImu.accel_b[0], mImu.accel_b[1], mImu.accel_b[2], mImu.gyro[0], mImu.gyro[1], mImu.gyro[2]);
	//ANO_DT_Send_Sensor(mImu.mag_b[0] * 1000, mImu.mag_b[1] * 1000, mImu.mag_b[2] * 1000, mImu.gyro[0] * 1000, mImu.gyro[1] * 1000, mImu.gyro[2]*1000,\
	//mImu.mag_raw[0] * 1000, mImu.mag_raw[1] * 1000, mImu.mag_raw[2] * 1000);
}

/////////////////////////////////////////////////////
//������ɷ���1
////////////////////////////////////////////////////
uint8_t  imu_caliSensorThread()
{
	static double gyrosum[3];
	static double accsum[3];
	float invNorm;
	float acctemp[3];
	
	static uint16_t  cnt = 0;
	static uint32_t tStart;
	if (cnt == 0)//������ʼ��
	{
		gyrosum[0] = gyrosum[1] = gyrosum[2] = 0.0f;
		accsum[0] = accsum[1] = accsum[2] = 0.0f;
		
		mImu.gyro_offset[0] = 0.0f;
		mImu.gyro_offset[1] = 0.0f;
		mImu.gyro_offset[2] = 0.0f;
#ifndef IMU_ACCEL_EILLPSOIL_FIT		//��ƫ����
		mImu.accel_offset[0] = 0.0f;
		mImu.accel_offset[1] = 0.0f;
		mImu.accel_offset[2] = 0.0f;
#endif
		tStart = millis();
	}

	gyrosum[0] += mImu.gyro_deg[0];
	gyrosum[1] += mImu.gyro_deg[1];
	gyrosum[2] += mImu.gyro_deg[2];

	accsum[0] += mImu.accel_b[0];
	accsum[1] += mImu.accel_b[1];
	accsum[2] += mImu.accel_b[2];
	cnt++;
	
	if ((millis()-tStart)>3000)
	{
		mImu.gyro_offset[0] = gyrosum[0] / cnt;
		mImu.gyro_offset[1] = gyrosum[1] / cnt;
		mImu.gyro_offset[2] = gyrosum[2] / cnt;
#ifdef IMU_ACCEL_EILLPSOIL_FIT				//���ٶȼ��������,��ȡ��ʼ״̬
		acctemp[0] = accsum[0]/cnt;
		acctemp[1] = accsum[1]/cnt;
		acctemp[2] = accsum[2]/cnt;
		invNorm = invSqrt(acctemp[0]*acctemp[0]+acctemp[1]*acctemp[1]+acctemp[2]*acctemp[2]);
		acctemp[0] *= invNorm;
		quat_initByEuler(&mImu.q,atan2f(acctemp[1],acctemp[2]),asinf(-acctemp[0]),0);
#else																	//���ٶȼ���ƫ
		mImu.accel_offset[0] = accsum[0] / cnt;
		mImu.accel_offset[1] = accsum[1] / cnt;
		mImu.accel_offset[2] = accsum[2] / cnt - CONSTANTS_ONE_G;
		quat_initByEuler(&mImu.q,0,0,0);
#endif		
		cnt = 0;
		return 1;
	}
	return 0;
}



/////////////////////////////////////////////////////
//��������imu����
////////////////////////////////////////////////////
void imu_update()
{
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	static uint8_t first = 1;
//========================================================
	imu_updateSensor();
	if (mImu.hasIMUReady == 0)
	{
		if(imu_caliSensorThread()==1)
		{
			mImu.hasIMUReady = 1;
		}
	}
	else
	{
			if (first){ tPre = micros(); first = 0; return;}
			tNow = micros();
			dt = (tNow - tPre) / 1000000.0f;
			tPre = tNow;
			//������Ԫ��	
			quat_update_complementy(&mImu.q, mImu.gyro[0], mImu.gyro[1], mImu.gyro[2], mImu.accel_b[0], mImu.accel_b[1], mImu.accel_b[2], dt);
			quaternion2dcm(&mImu.q, mImu.dcm);													//����ת������
			dcm2angle(mImu.dcm, mImu.orientation);											//����ŷ����
			rad2deg(mImu.orientation_deg, mImu.orientation, 3);					//ŷ����תΪ��

			body2world(mImu.dcm,mImu.accel_b,mImu.accel_w);
			mImu.accel_w[2] -= CONSTANTS_ONE_G;
			
	}
}



/////////////////////////////////////////////////////
//��Ԫ��ת�仯����R:body->world
////////////////////////////////////////////////////
void quaternion2dcm(quaternion *q, float R[3][3])
{
	//��һ��
	R[0][0] = 1.0f - 2.0f*(q->q2q2 + q->q3q3);
	R[0][1] = 2.0f*(q->q1q2 - q->q0q3);
	R[0][2] = 2.0f*(q->q1q3 + q->q0q2);
	//�ڶ���
	R[1][0] = 2.0f*(q->q1q2 + q->q0q3);
	R[1][1] = 1.0f - 2.0f*(q->q1q1 + q->q3q3);
	R[1][2] = 2.0f*(q->q2q3 - q->q0q1);
	//������
	R[2][0] = 2.0f*(q->q1q3 - q->q0q2);
	R[2][1] = 2.0f*(q->q2q3 + q->q0q1);
	R[2][2] = 1.0f - 2.0f*(q->q1q1 + q->q2q2);
}


/////////////////////////////////////////////////////
//ת������Rתŷ����,rad
////////////////////////////////////////////////////
void dcm2angle(float R[3][3],float angel[3])
{
	angel[0] = atan2f(R[2][1], R[2][2]);
	angel[1] = -asinf(R[2][0]);
	angel[2] = atan2f(R[1][0], R[0][0]);
}



/////////////////////////////////////////////////////
//ͨ��ת����������������
////////////////////////////////////////////////////
void imu_getGravity(float g[3],float R[3][3])
{
	g[0] = CONSTANTS_ONE_G*R[2][0];
	g[1] = CONSTANTS_ONE_G*R[2][1];
	g[2] = CONSTANTS_ONE_G*R[2][2];
}

/////////////////////////////////////////////////////
//�������ӻ�������ϵ->��������ϵ
////////////////////////////////////////////////////
void body2world(float R[3][3],float body[3], float world[3])
{
	//matrix_mv(world, R, body, 3, 3);
	world[0] = R[0][0] * body[0] + R[0][1] * body[1] + R[0][2] * body[2];
	world[1] = R[1][0] * body[0] + R[1][1] * body[1] + R[1][2] * body[2];
	world[2] = R[2][0] * body[0] + R[2][1] * body[1] + R[2][2] * body[2];
}

void world2body(float R[3][3], float world[3], float body[3])
{
	body[0] = R[0][0] * world[0] + R[1][0] * world[1] + R[2][0] * world[2];
	body[1] = R[0][1] * world[0] + R[1][1] * world[1] + R[2][1] * world[2];
	body[2] = R[0][2] * world[0] + R[1][2] * world[1] + R[2][2] * world[2];
}

