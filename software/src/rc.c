#include "rc.h"
#include "sys.h"

#include "flight.h"
#include "ano.h"
#include "bytes.h"
#include "imu.h"
#include "cmath.h"
#include "controller.h"
#include "bytes.h"
#include "table.h"

rc_t mRC;
static uint8_t rc_buf[32];


void rc_init()
{
	mRC.channel[CH_THRO] = 1000;
	mRC.channel[CH_ROLL] = 1500;
	mRC.channel[CH_PITCH] = 1500;
	mRC.channel[CH_YAWRATE] = 1500;
	mRC.channel[CH_GEAR] = 2000;
	mRC.channel[CH_6] = 2000;

	
	mRC.thro = 0.0f;
	mRC.roll = 0.0f;
	mRC.pitch = 0.0f;
	mRC.yaw_rate = 0.0f;
}

void rc_update()
{
	uint16_t  len;

	static uint32_t tPre = 0;
	static uint8_t first = 1;
	uint32_t  tNow;
	if (first){ tPre = millis(); first = 0; }
	tNow = millis();

	len = nrf24l01_recvPacket(rc_buf);
	if (len == 0)
	{
		if ((tNow - tPre) > RC_TIMEOUT)
		{
			mFlight.lostRadio = 1;
	#if RC_TIMEOUT_ENABLE == 1
			flight_lock();
	#endif
		}
		return;
	}
	//ledM = !ledM;
	mFlight.recvRCData = 1;
	mFlight.lostRadio = 0;
	ANO_RecvBytes(rc_buf, len);	 //ANO协议解析,
	tPre = millis();			//记录最近接受数据时间
}

#define PID_SET_ALL			//所以方向PID与Roll相同
void ANO_RecvData_CallBack(uint8_t cmd,uint8_t *buf, uint8_t len)
{
//	uint8_t fRecv = 1;
	switch (cmd)
	{
	case ANO_CHANNELVALUE:
		mRC.channel[CH_ROLL] = BigBytesToINT16(buf); buf += 2;
		mRC.channel[CH_PITCH] = BigBytesToINT16(buf); buf += 2;
		mRC.channel[CH_THRO] = BigBytesToINT16(buf); buf += 2;
		mRC.channel[CH_YAWRATE] = BigBytesToINT16(buf); buf += 2;
		mRC.channel[CH_GEAR] = BigBytesToINT16(buf); buf += 2;
		mRC.channel[CH_6] = BigBytesToINT16(buf); buf += 2;
	break;
	//=================================================================================
	case CSP_ARM_IT:		//上锁		
			flight_lock();
	break;
	case CSP_DISARM_IT:	//解锁
		if(mImu.hasIMUReady == 1)				//传感器初始化完成才允许解锁
			flight_unlock();
	break;
	case CSP_SENSOR_CALI:				//传感器校准
		mImu.hasIMUReady = 0;			//加速度计,陀螺仪校准	
	break;		
	case CSP_HOLD_ALT:
		mFlight.flyMode = FLMODE_HOLD_ALT;
	break;
	case CSP_STOP_HOLD_ALT:
		mFlight.flyMode = FLMODE_ATTITUDE;
	break;
//=设置命令=====================================================
	case CSP_PID1:
			pidRollRate.kp = (float)BytesToUINT16(buf)/1000;
			pidRollRate.ki = (float)BytesToUINT16(buf+2)/1000;
			pidRollRate.kd = (float)BytesToUINT16(buf+4)/1000;
			pospid_reset(&pidRollRate);
	
#ifdef PID_SET_ALL
			pidPitchRate.kp = (float)BytesToUINT16(buf)/1000;
			pidPitchRate.ki = (float)BytesToUINT16(buf+2)/1000;
			pidPitchRate.kd = (float)BytesToUINT16(buf+4)/1000;
			pospid_reset(&pidPitchRate);	
	
			pidYawRate.kp = (float)BytesToUINT16(buf)/1000;
			pidYawRate.ki = (float)BytesToUINT16(buf+2)/1000;
			pidYawRate.kd = (float)BytesToUINT16(buf+4)/1000;
			pospid_reset(&pidYawRate);	
#endif
	break;
	case CSP_PID2:
			pidRollAngle.kp = (float)BytesToUINT16(buf)/1000;
			pidRollAngle.ki = (float)BytesToUINT16(buf+2)/1000;
			pidRollAngle.kd = (float)BytesToUINT16(buf+4)/1000;
			pospid_reset(&pidRollAngle);	

#ifdef PID_SET_ALL
			pidPitchAngle.kp = (float)BytesToUINT16(buf)/1000;
			pidPitchAngle.ki = (float)BytesToUINT16(buf+2)/1000;
			pidPitchAngle.kd = (float)BytesToUINT16(buf+4)/1000;		
			pospid_reset(&pidPitchAngle);	
	
			pidYawAngle.kp = (float)BytesToUINT16(buf)/1000;
			pidYawAngle.ki = (float)BytesToUINT16(buf+2)/1000;
			pidYawAngle.kd = (float)BytesToUINT16(buf+4)/1000;		
			pospid_reset(&pidYawAngle);	
#endif
	
	break;
	case CSP_PID3:
		pidPitchRate.kp = (float)BytesToUINT16(buf)/1000;
		pidPitchRate.ki = (float)BytesToUINT16(buf+2)/1000;
		pidPitchRate.kd = (float)BytesToUINT16(buf+4)/1000;
		pospid_reset(&pidPitchRate);	
	break;
	case CSP_PID4:			
		pidPitchAngle.kp = (float)BytesToUINT16(buf)/1000;
		pidPitchAngle.ki = (float)BytesToUINT16(buf+2)/1000;
		pidPitchAngle.kd = (float)BytesToUINT16(buf+4)/1000;		
		pospid_reset(&pidPitchAngle);	
	break;		
	case CSP_PID5:
		pidYawRate.kp = (float)BytesToUINT16(buf)/1000;
		pidYawRate.ki = (float)BytesToUINT16(buf+2)/1000;
		pidYawRate.kd = (float)BytesToUINT16(buf+4)/1000;
		pospid_reset(&pidYawRate);	
	break;		
	case CSP_PID6:
		pidYawAngle.kp = (float)BytesToUINT16(buf)/1000;
		pidYawAngle.ki = (float)BytesToUINT16(buf+2)/1000;
		pidYawAngle.kd = (float)BytesToUINT16(buf+4)/1000;		
		pospid_reset(&pidYawAngle);	
	break;		
	case CSP_PID7:
		pidAltRate.kp = (float)BytesToUINT16(buf)/1000;
		pidAltRate.ki = (float)BytesToUINT16(buf+2)/1000;
		pidAltRate.kd = (float)BytesToUINT16(buf+4)/1000;	
		pospid_reset(&pidAltRate);		
	break;		
	case CSP_PID8:		
		pidAlt.kp = (float)BytesToUINT16(buf)/1000;
		pidAlt.ki = (float)BytesToUINT16(buf+2)/1000;
		pidAlt.kd = (float)BytesToUINT16(buf+4)/1000;		
		pospid_reset(&pidAlt);	
	break;
	
	case CSP_SET_ROLLERR:
 		mTab.roll_zero  = (float)BytesToINT16(buf)/10;
 	break;
 	case CSP_SET_PITCHERR:		
 		mTab.pitch_zero = (float)BytesToINT16(buf)/10;
 	break;	
//=========================================================
	case 	CSP_STARTIAP:		//IAP
	//iap_gotoApp(FLASH_ISP_ADDR);
	break;	
//==============================
	default:
//		fRecv = 0;		
	break;
	}
	
	//printf("=>gear0=%d,rudd0=%d\r\n",mRC.channel[CH_GEAR],mRC.channel[CH_6]);
	//debug("=>thro=%d,yaw=%d,roll=%d,pitch=%d\r\n", mRC.channel[CH_THRO], mRC.channel[CH_YAWRATE], mRC.channel[CH_ROLL], mRC.channel[CH_PITCH]);
}


//数据归一化
float rc_dbScaleLinear(float x, float x_end, float deadband)
{
	if (x > deadband) {
		return (x - deadband) / (x_end - deadband);

	}
	else if (x < -deadband) {
		return (x + deadband) / (x_end - deadband);

	}
	else {
		return 0.0f;
	}
}


void rc_dealRCData()
{
	//双向限幅
	mRC.channel[CH_THRO] = RANGEVALUE(mRC.channel[CH_THRO], 1000, 2000);
	mRC.channel[CH_YAWRATE] = RANGEVALUE(mRC.channel[CH_YAWRATE], 1000, 2000);
	mRC.channel[CH_PITCH] = RANGEVALUE(mRC.channel[CH_PITCH], 1000, 2000);
	mRC.channel[CH_ROLL] = RANGEVALUE(mRC.channel[CH_ROLL], 1000, 2000);

	/////////////////////////////////////////////////////
	//摇杆手势识别:
	//油门最小,ROLL最大->解锁
	//油门最小,ROLL最小->上锁
	//油门最小,PITCH最大->校准
	////////////////////////////////////////////////////
	if (mRC.channel[CH_THRO] < RC_MINLINE)
	{	
		if (mRC.channel[CH_PITCH] > RC_MAXLINE)
		{
			mImu.hasIMUReady = 0;
		}
		if (mRC.channel[CH_ROLL] > RC_MAXLINE)
		{
			if (mImu.hasIMUReady == 1)
				flight_unlock();
		}
		else if (mRC.channel[CH_ROLL] < RC_MINLINE)
		{
			flight_lock();
		}
	}
	
	if(mRC.channel[CH_6]==2000)
	{
		mFlight.flyMode = FLMODE_HOLD_ALT;
		pospid_reset(&pidAltRate);
		pospid_reset(&pidAlt);
		mController.alt_target = 0.5f;
		
	}
	else
	{
		mFlight.flyMode = FLMODE_ATTITUDE;
	}
	
	//提取数据
	mRC.thro = mRC.channel[CH_THRO] - 1000;
	mRC.pitch = RC_ANGLEMAX*rc_dbScaleLinear(mRC.channel[CH_PITCH] - 1500, 500, RC_DB);
	mRC.roll = RC_ANGLEMAX*rc_dbScaleLinear(mRC.channel[CH_ROLL] - 1500, 500, RC_DB);
	mRC.yaw_rate = RC_YAWRATEMAX*rc_dbScaleLinear(mRC.channel[CH_YAWRATE] - 1500, 500, RC_DB);

//  debug("yaw=%.2f,thro=%.2f,roll=%.2f,pitch=%.2f\r\n", mRC.yaw_rate, mRC.thro, mRC.roll,mRC.pitch);
}


