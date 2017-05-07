#include "flight.h"
#include "sys.h"

#include "imu.h"
#include "controller.h"


flight_t mFlight;


void flight_init()
{
	mFlight.lostRadio = 1;
	mFlight.flyMode = FLMODE_ATTITUDE;
	mFlight.flyEnable = 0;
	mFlight.recvRCData = 0;
}



void flight_safeControl()
{
	if(mImu.orientation_deg[0] > FL_ANGEL_MAX || mImu.orientation_deg[1] > FL_ANGEL_MAX
		|| mImu.orientation_deg[0] < -FL_ANGEL_MAX || mImu.orientation_deg[1] < -FL_ANGEL_MAX)
	{
		mFlight.flyEnable = 0;
	}
}



//解锁
void flight_unlock()
{
	//解锁准备
	pospid_reset(&pidRollRate);
	pospid_reset(&pidPitchRate);
	pospid_reset(&pidYawRate);
	
	pospid_reset(&pidRollAngle);
	pospid_reset(&pidPitchAngle);
	pospid_reset(&pidYawAngle);
	
	pospid_reset(&pidAltRate);
	pospid_reset(&pidAlt);
	
	mController.angle_target[2] = mImu.orientation_deg[2];
	
	mFlight.flyEnable = 1;
	
}

void flight_lock()		//上锁
{
		mFlight.flyEnable = 0;
}
