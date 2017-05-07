#include "controller.h"
#include "sys.h"

#include "imu.h"
#include "rc.h"
#include "flight.h"
#include "ano.h"
#include "math.h"
#include "cmath.h"
#include "filter.h"
#include "alt.h"


pid_t pidPitchAngle, pidRollAngle, pidYawAngle;		//姿态外环pid
pid_t pidPitchRate, pidRollRate, pidYawRate;			//姿态内环
pid_t pidAlt, pidAltRate;													//高度,z轴速率
pid_t pidPosX, pidPosY;														//位置pid


controller_t mController;


void controller_init()
{

	/*控制量清零*/
	mController.thro = 0.0f;
	mController.pitch = 0.0f;
	mController.roll = 0.0f;
	mController.yaw = 0.0f;
	
	/*姿态内环pid初始化化*/
	pospid_init(&pidRollRate, mTab.RollRate_Kp,mTab.RollRate_Ki,mTab.RollRate_Kd, ATTI_RATE_INT_LIMIT);
	pospid_init(&pidPitchRate, mTab.PitchRate_Kp,mTab.PitchRate_Ki,mTab.PitchRate_Kd, ATTI_RATE_INT_LIMIT);
	pospid_init(&pidYawRate, mTab.YawRate_Kp,mTab.YawRate_Ki,mTab.YawRate_Kd,ATTI_RATE_INT_LIMIT);

	/*姿态外环pid初始化*/
	pospid_init(&pidRollAngle, mTab.RollAngle_Kp,mTab.RollAngle_Ki,mTab.RollAngle_Kd,ANGLE_INT_LIMIT);
	pospid_init(&pidPitchAngle, mTab.PitchAngle_Kp,mTab.PitchAngle_Ki,mTab.PitchAngle_Kd, ANGLE_INT_LIMIT);
	pospid_init(&pidYawAngle, mTab.YawAngle_Kp,mTab.YawAngle_Ki,mTab.YawAngle_Kd,ANGLE_INT_LIMIT);

	/*高度pid初始化*/
	pospid_init(&pidAltRate,mTab.AltRate_Kp,mTab.AltRate_Ki,mTab.AltRate_Kd,ALT_RATE_INT_LIMIT);
	pospid_init(&pidAlt,mTab.Alt_Kp,mTab.Alt_Ki,mTab.Alt_Kd,ALT_INT_LIMIT);

	
	/*期望初始化*/
	mController.atti_rate_target[0] = 0.0f;
	mController.atti_rate_target[1] = 0.0f;
	mController.atti_rate_target[2] = 0.0f;
	
	mController.angle_target[0] = 0.0f;
	mController.angle_target[1] = 0.0f;
	mController.angle_target[2] = 0.0f;
	
	mController.alt_rate_target = 0.0f;
	mController.alt_target = 0.0f;

	mController.alt_thro = 0.0f;
}

void controller_updateThro(float dt,uint8_t boost)
{

	if(mFlight.flyMode == FLMODE_ATTITUDE)
	{
		static float thro_temp = 0.0f;
		thro_temp += FILTER1ST_COEF(20.0f,dt)*(mRC.thro - thro_temp);	
		if(boost)
		{
			mController.thro = thro_temp/RANGEVALUE(mImu.dcm[2][2],0.5f,1.0f);
		}
		else
		{
			mController.thro = thro_temp;
		}
	}
	else if(mFlight.flyMode == FLMODE_HOLD_ALT)
	{
		float thro_temp;
		thro_temp = CTRL_THRO_HOLDALT_BASE+mController.alt_thro;
		mController.thro = thro_temp/RANGEVALUE(mImu.dcm[2][2],0.5f,1.0f);;
	}
	
}



/////////////////////////////////////////////////////
//姿态内环,直接输出到到动力
////////////////////////////////////////////////////
void controller_dealAttiRate()
{
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	float err[3];

	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }

	tNow = micros();
	dt = (tNow - tPre) / 1000000.0f;
	tPre = tNow;
	
	
	err[0] = mController.atti_rate_target[0] - mImu.gyro_deg[0];
	err[1] = mController.atti_rate_target[1] - mImu.gyro_deg[1];
	err[2] = mController.atti_rate_target[2] - mImu.gyro_deg[2];
	
	mController.roll =  pid_getPID(&pidRollRate,err[0],dt);
	mController.pitch =  pid_getPID(&pidPitchRate,err[1],dt);
	mController.yaw =  pid_getPID(&pidYawRate,err[2],dt);
	
	//输出限幅
	mController.roll = RANGEVALUE(mController.roll,-CTRL_ATTI_RATE_PR_OUT_MAX,CTRL_ATTI_RATE_PR_OUT_MAX);
	mController.pitch = RANGEVALUE(mController.pitch,-CTRL_ATTI_RATE_PR_OUT_MAX,CTRL_ATTI_RATE_PR_OUT_MAX);
	mController.yaw = RANGEVALUE(mController.yaw,-CTRL_ATTI_RATE_YAW_OUT_MAX,CTRL_ATTI_RATE_YAW_OUT_MAX);	
//	debug("pid: tar=%.2f,err=%.2f,out=%.2f\r\n",mController.rate_target[1],err[1],mController.pitch);
}



void controller_dealAtti()
{
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	float err[3];
	
	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }

	tNow = micros();
	dt = (tNow - tPre) / 1000000.0f;
	tPre = tNow;
	
	//期望
	mController.angle_target[0] = mRC.roll+mTab.roll_zero;
	mController.angle_target[1] = mRC.pitch+mTab.pitch_zero;
	mController.angle_target[2] += mRC.yaw_rate*dt;

	//误差
	err[0] = mywrap180(mController.angle_target[0] - mImu.orientation_deg[0]);
	err[1] = mywrap180(mController.angle_target[1] - mImu.orientation_deg[1]);
	err[2] = mywrap180(mController.angle_target[2] - mImu.orientation_deg[2]);

	//PID
	mController.atti_rate_target[0] = pid_getPID(&pidRollAngle,err[0],dt);
	mController.atti_rate_target[1] = pid_getPID(&pidPitchAngle,err[1],dt);
	mController.atti_rate_target[2] = pid_getPID(&pidYawAngle,err[2],dt);
	
	//限制最大旋转速度,度/s
	mController.atti_rate_target[0] = RANGEVALUE(mController.atti_rate_target[0],-CTRL_ATTI_RATE_EXP_MAX,CTRL_ATTI_RATE_EXP_MAX);	
	mController.atti_rate_target[1] = RANGEVALUE(mController.atti_rate_target[1],-CTRL_ATTI_RATE_EXP_MAX,CTRL_ATTI_RATE_EXP_MAX);
	mController.atti_rate_target[2] = RANGEVALUE(mController.atti_rate_target[2],-CTRL_ATTI_RATE_EXP_MAX,CTRL_ATTI_RATE_EXP_MAX);
}



void controller_dealAltRate()
{
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	float err;
	
	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }

	tNow = micros();
	dt = (tNow - tPre) / 1000000.0f;
	tPre = tNow;
	
	err = 100.0f*(mController.alt_rate_target - mImu.velocity[2]);
	
	mController.alt_thro = pid_getPID2(&pidAltRate,err,dt);		//附加油门
	mController.alt_thro = RANGEVALUE(mController.alt_thro,-CTRL_ALT_RATE_OUT_MAX,CTRL_ALT_RATE_OUT_MAX);
	
	pidAltRate.measure = mImu.velocity[2]*100;
	pidAltRate.desired = mController.alt_rate_target*100;
}


void controller_dealAlt()
{
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	float err;
	
	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }

	tNow = micros();
	dt = (tNow - tPre) / 1000000.0f;
	tPre = tNow;
	
	err = (mController.alt_target - mImu.position[2]);
	mController.alt_rate_target = pid_getPID2(&pidAlt,err,dt);		
	//mController.alt_rate_target = RANGEVALUE(mController.alt_rate_target,-CTRL_ALT_RATE_EXP_MAX,CTRL_ALT_RATE_EXP_MAX);
}


void controller_setMotors()
{
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }

	tNow = micros();
	dt = (tNow - tPre) / 1000000.0f;
	tPre = tNow;
	
	
	if((mFlight.flyEnable == 1) && mRC.thro > CTRL_THRO_LAND)		//解锁且没有关闭油门
	{
		
		mController.power[MO_HL] =  (mController.pitch) + (mController.roll) - mController.yaw;  
		mController.power[MO_TL] = -(mController.pitch) + (mController.roll) + mController.yaw;
		mController.power[MO_TR] = -(mController.pitch) - (mController.roll) - mController.yaw;
		mController.power[MO_HR] =  (mController.pitch) - (mController.roll) + mController.yaw;
		

		controller_updateThro(dt,1);							//更新油门
		
		mController.power[MO_HL] += mController.thro;
		mController.power[MO_TL] += mController.thro;
		mController.power[MO_TR] += mController.thro;
		mController.power[MO_HR] += mController.thro;
		
		//油门限幅
		mController.motor[MO_HL] = (uint16_t)RANGEVALUE(mController.power[MO_HL],0.0f,1000.0f);
		mController.motor[MO_TL] = (uint16_t)RANGEVALUE(mController.power[MO_TL],0.0f,1000.0f);
		mController.motor[MO_TR] = (uint16_t)RANGEVALUE(mController.power[MO_TR],0.0f,1000.0f);
		mController.motor[MO_HR] = (uint16_t)RANGEVALUE(mController.power[MO_HR],0.0f,1000.0f);
	}
	else
	{
		mController.motor[0] = mController.motor[1] = mController.motor[2] = mController.motor[3] = 0;
	}
	//ANO_DT_Send_MotoPWM(mController.motor[MO_HR], mController.motor[MO_HL], mController.motor[MO_TL],mController.motor[MO_TR],0, 0, 0, 0);
	motor_set(mController.motor[MO_HL], mController.motor[MO_HR], mController.motor[MO_TL], mController.motor[MO_TR]);
}



