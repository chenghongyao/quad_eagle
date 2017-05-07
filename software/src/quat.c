#include "quat.h"
#include "sys.h"

#include "cmath.h"
#include "math.h"
static float exInt,eyInt,ezInt;
	
	
void quat_normalized(float oq[4], float q[4])
{
	
	float invNormal = invSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	oq[0] = q[0] * invNormal;
	oq[1] = q[1] * invNormal;
	oq[2] = q[2] * invNormal;
	oq[3] = q[3] * invNormal;
}


void quat_prepare(quaternion *q)
{
	q->q0q0 = q->q[0] * q->q[0];
	q->q0q1 = q->q[0] * q->q[1];
	q->q0q2 = q->q[0] * q->q[2];
	q->q0q3 = q->q[0] * q->q[3];

	q->q1q1 = q->q[1] * q->q[1];
	q->q1q2 = q->q[1] * q->q[2];
	q->q1q3 = q->q[1] * q->q[3];

	q->q2q2 = q->q[2] * q->q[2];
	q->q2q3 = q->q[2] * q->q[3];

	q->q3q3 = q->q[3] * q->q[3];
	
}


void quat_reset(quaternion *q)
{
	q->q[0] = 1.0f;
	q->q[1] = q->q[2] = q->q[3] = 0.0f;
	quat_prepare(q);
	exInt = eyInt = ezInt = 0.0f;
}


void quat_initByEuler(quaternion *q,float roll,float pitch,float yaw)
{
	float cos_roll,cos_pitch,cos_yaw;
	float sin_roll,sin_pitch,sin_yaw;
	
	roll *= 0.5f;
	pitch *= 0.5f;
	yaw *= 0.5f;
	
	cos_roll = cosf(roll);
	cos_pitch = cosf(pitch);
	cos_yaw = cosf(yaw);
	sin_roll = sinf(roll);
	sin_pitch = sinf(pitch);
	sin_yaw = sinf(yaw);
	
	q->q[0] = cos_roll*cos_pitch*cos_yaw+sin_roll*sin_pitch*sin_yaw;
	q->q[1] = sin_roll*cos_pitch*cos_yaw+cos_roll*sin_pitch*sin_yaw;
	q->q[2] = cos_roll*sin_pitch*cos_yaw+sin_roll*cos_pitch*sin_yaw;
	q->q[3] = cos_roll*cos_pitch*sin_yaw+sin_roll*sin_pitch*cos_yaw;
	
	exInt = eyInt = ezInt = 0.0f;
	quat_prepare(q);
}

/////////////////////////////////////////////////////
// 一阶毕卡尔
////////////////////////////////////////////////////
void quat_update_1st(quaternion *q,float gx,float gy,float gz,float dt)
{
	float qtemp[4];
	float dx = gx * dt;
	float dy = gy * dt;
	float dz = gz * dt;

	qtemp[0] = q->q[0] + 0.5f*(q->q[1] * (-dx) + q->q[2] * (-dy) + q->q[3] * (-dz));
	qtemp[1] = q->q[1] + 0.5f*(q->q[0] * (dx)  + q->q[2] * (dz)  + q->q[3] * (-dy));
	qtemp[2] = q->q[2] + 0.5f*(q->q[0] * (dy)  + q->q[1] * (-dz) + q->q[3] * (dx));
	qtemp[3] = q->q[3] + 0.5f*(q->q[0] * (dz)  + q->q[1] * (dy)  + q->q[2] * (-dx));

	quat_normalized(q->q, qtemp);
	quat_prepare(q);

}

/////////////////////////////////////////////////////
// 二阶毕卡尔
////////////////////////////////////////////////////
void quat_update_2nd(quaternion *q, float gx,float gy,float gz, float dt)
{
	float qtemp[4];
	float dx = gx * dt;
	float dy = gy * dt;
	float dz = gz * dt;
	float dtheta2 = dx*dx + dy*dy + dz*dz;
	float ratio = (1 - dtheta2 / 8.0f);

	qtemp[0] = ratio * q->q[0] + 0.5f*(q->q[1] * (-dx) + q->q[2] * (-dy) + q->q[3] * (-dz));
	qtemp[1] = ratio * q->q[1] + 0.5f*(q->q[0] * (dx)+q->q[2] * (dz)+q->q[3] * (-dy));
	qtemp[2] = ratio * q->q[2] + 0.5f*(q->q[0] * (dy)+q->q[1] * (-dz) + q->q[3] * (dx));
	qtemp[3] = ratio * q->q[3] + 0.5f*(q->q[0] * (dz)+q->q[1] * (dy)+q->q[2] * (-dx));

	quat_normalized(q->q, qtemp);
	quat_prepare(q);
}

/////////////////////////////////////////////////////
// 三阶毕卡尔
////////////////////////////////////////////////////
void quat_update_3rd(quaternion *q, float *gyro, float dt)
{
	float qtemp[4];
	float dx = gyro[0]*dt;
	float dy = gyro[1]*dt;
	float dz = gyro[2]*dt;
	float dtheta2 = dx*dx + dy*dy + dz*dz;
	float ratio = (1.0f - dtheta2 / 8.0f);
	float ratiodot = (0.5f - dtheta2 / 48.0f);

	qtemp[0] = ratio * q->q[0] + ratiodot*(q->q[1] * (-dx) + q->q[2] * (-dy) + q->q[3] * (-dz));
	qtemp[1] = ratio * q->q[1] + ratiodot*(q->q[0] * (dx)+q->q[2] * (dz)+q->q[3] * (-dy));
	qtemp[2] = ratio * q->q[2] + ratiodot*(q->q[0] * (dy)+q->q[1] * (-dz) + q->q[3] * (dx));
	qtemp[3] = ratio * q->q[3] + ratiodot*(q->q[0] * (dz)+q->q[1] * (dy)+q->q[2] * (-dx));

	quat_normalized(q->q, qtemp);
	quat_prepare(q);
}



#define X_DIM	4
#define Z_DIM	3

void quat_update_ekf(quaternion *q, float gx, float gy, float gz, float ax, float ay, float az, float dt)
{

	float X[X_DIM];
	float x_cor[X_DIM];
	float A[X_DIM*X_DIM], AT[X_DIM*X_DIM];
	float cov_temp[X_DIM*X_DIM];
	static float cov_pre[X_DIM*X_DIM] = { 0.1f, 0.0f, 0.0f, 0.0f,
																				0.0f, 0.1f, 0.0f, 0.0f,
																				0.0f, 0.0f, 0.1f, 0.0f,
																				0.0f, 0.0f, 0.0f, 0.1f,};
	float Z[Z_DIM];
	float z_cor[Z_DIM];
	float H[Z_DIM*X_DIM], HT[X_DIM*Z_DIM];
	float cov_z[Z_DIM*Z_DIM];
	float invcov_z[Z_DIM*Z_DIM];

	float K[X_DIM*Z_DIM];		//卡尔曼增益
	static float R = 10.1f;		//过程噪声
	static float Q = 300.5f;		//测量噪声

	
	float mattemp_XbX[X_DIM*X_DIM];		//辅助矩阵
	float mattemp_XbZ[X_DIM*Z_DIM];		//辅助矩阵

	
	float hdx, hdy, hdz;
	float invNormal;
	uint16_t index;

	hdx = 0.5 * gx * dt;
	hdy = 0.5 * gy * dt;
	hdz = 0.5 * gz * dt;

	//Z
	invNormal = invSqrt(ax * ax + ay * ay + az * az);
 	Z[0] = ax*invNormal;
 	Z[1] = ay*invNormal;
 	Z[2] = az*invNormal;

	//^^^A,AT
	A[0] = 1.0f;	A[1] = -hdx;	A[2] = -hdy;	A[3] = -hdz;
	A[4] = hdx;		A[5] = 1.0f;	A[6] = hdz;		A[7] = -hdy;
	A[8] = hdy;		A[9] = -hdz;	A[10] = 1.0f;	A[11] = hdx;
	A[12] = hdz;	A[13] = hdy;	A[14] = -hdx;	A[15] = 1.0f;
	matrix_transpose(AT, A, X_DIM, X_DIM);

	//^^^H,HT
	H[0] = -2.0f*q->q[2];  H[1] =  2.0f*q->q[3];  H[2] = -2.0f*q->q[0];  H[3] = 2.0f*q->q[1];
	H[4] =  2.0f*q->q[1];  H[5] =  2.0f*q->q[0];  H[6] =  2.0f*q->q[3];  H[7] = 2.0f*q->q[2];
	H[8] = 0.0f;  H[9] = -2.0f*2.0f*q->q[1];  H[10] = -2.0f*2.0f*q->q[2];  H[11] = 0.0f;
	matrix_transpose(HT, H, Z_DIM, X_DIM);


	//^^^先验 状态 g(x(k))
	matrix_mv(X,A,q->q,X_DIM,X_DIM);
//	X[0] = q->q[0] + (q->q[1] * (-hdx) + q->q[2] * (-hdy) + q->q[3] * (-hdz));
//	X[1] = q->q[1] + (q->q[0] * (hdx)+q->q[2] * (hdz)+q->q[3] * (-hdy));
//	X[2] = q->q[2] + (q->q[0] * (hdy)+q->q[1] * (-hdz) + q->q[3] * (hdx));
//	X[3] = q->q[3] + (q->q[0] * (hdz)+q->q[1] * (hdy)+q->q[2] * (-hdx));

//*
	//先验协方差
	matrix_dot(mattemp_XbX, A, cov_pre, X_DIM, X_DIM, X_DIM);
	matrix_dot(cov_temp, mattemp_XbX, AT, X_DIM, X_DIM, X_DIM);
	index = 0;			//^^^
	cov_temp[index] += R;	index += X_DIM + 1;
	cov_temp[index] += R;	index += X_DIM + 1;
	cov_temp[index] += R;	index += X_DIM + 1;
	cov_temp[index] += R;	//index += X_DIM + 1;


	//卡尔曼增益
	matrix_dot(mattemp_XbZ, cov_temp, HT, X_DIM, X_DIM, Z_DIM);
	matrix_dot(cov_z, H, mattemp_XbZ, Z_DIM, X_DIM, Z_DIM);
	index = 0;			//^^^
	cov_z[index] += Q;	index += Z_DIM + 1;
	cov_z[index] += Q;	index += Z_DIM + 1;
	cov_z[index] += Q;	//index += Z_DIM + 1;

	matrix_inv3b3(invcov_z, cov_z);			//^^^
	matrix_dot(K, mattemp_XbZ, invcov_z, X_DIM, Z_DIM, Z_DIM);

	//^^^后验状态 h()
	z_cor[0] = 2.0f*(X[1]*X[3]-X[0]*X[2]);
	z_cor[1] = 2.0f*(X[0]*X[1]+X[2]*X[3]);
	z_cor[2] = 1.0f-2.0f*(X[1]*X[1]+X[2]*X[2]);
	vector_subtract(NULL, Z, z_cor, Z_DIM);
	
	matrix_mv(x_cor, K, Z, X_DIM, Z_DIM);
	vector_add(NULL, X, x_cor,X_DIM);				

	//后验协方差
	matrix_dot(mattemp_XbZ, H, cov_temp, Z_DIM, X_DIM, X_DIM);	//ZbX
	matrix_dot(mattemp_XbX, K, mattemp_XbZ, X_DIM, Z_DIM, X_DIM);
	vector_subtract(cov_pre, cov_temp, mattemp_XbX,X_DIM*X_DIM);
//*/

	//单位四元数
	quat_normalized(q->q, X);

	q->q0q0 = q->q[0] * q->q[0];
	q->q0q1 = q->q[0] * q->q[1];
	q->q0q2 = q->q[0] * q->q[2];
	q->q0q3 = q->q[0] * q->q[3];

	q->q1q1 = q->q[1] * q->q[1];
	q->q1q2 = q->q[1] * q->q[2];
	q->q1q3 = q->q[1] * q->q[3];

	q->q2q2 = q->q[2] * q->q[2];
	q->q2q3 = q->q[2] * q->q[3];

	q->q3q3 = q->q[3] * q->q[3];
}



void quat_update_complementy(quaternion *q, float gx, float gy, float gz, float ax, float ay, float az, float dt)
{
	float invNormal;
 	float vx, vy, vz;
 	float ex,ey,ez;
 
 	invNormal = invSqrt(ax * ax + ay * ay + az * az);
 	ax *= invNormal;
 	ay *= invNormal;
 	az *= invNormal;
 

	vx = 2.0f*(q->q1q3 - q->q0q2);
	vy = 2.0f*(q->q2q3 + q->q0q1);
	vz = q->q0q0 - q->q1q1 - q->q2q2 + q->q3q3;

 	ex = ay * vz - az * vy;
 	ey = az * vx - ax * vz;
 	ez = ax * vy - ay * vx;
 //比例
  gx +=  QUAT_COMP_KP*ex;
 	gy +=  QUAT_COMP_KP*ey;
 	gz +=  QUAT_COMP_KP*ez;
	//积分
 	exInt += ex*dt;
 	eyInt += ey*dt;
 	ezInt += ez*dt;
	exInt = RANGEVALUE(exInt,-10,10);		//积分限幅
	eyInt = RANGEVALUE(exInt,-10,10);
	ezInt = RANGEVALUE(exInt,-10,10);
	
	gx += QUAT_COMP_KI*exInt;
	gy += QUAT_COMP_KI*eyInt;
	gz += QUAT_COMP_KI*ezInt;
	
	quat_update_1st(q,gx,gy,gz,dt);
}
