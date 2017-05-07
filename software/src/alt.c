#include "alt.h"
#include "sys.h"
#include "cmath.h"
#include "ekf.h"
#include "ultrasound.h"
#include "imu.h"
#include "ano.h"

#include "filter.h"
//alt_t mAlt;




void alt_update_ekf()
{	
	static float R[2];
	static float Q;
	
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }
	
	tNow = micros();
	dt = (tNow - tPre) / 1000000.0f;
	tPre = tNow;
	
	if(mImu.hasIMUReady==0)return;
	
	/*¹ý³ÌÔëÉù*/
	R[0] = 3.47e-6f;
	R[1] = 7.47e-5f;
	
	/*(³¬Éù²¨)²âÁ¿ÔëÉù*/
	if(mUltra.distance>3.0f)
	{	 
		Q = 10.0f;		
	}
	else if(mUltra.distance>4.0f)
	{
		Q = 1000.0f;
	}
	else
	{
		Q = 0.005;
	}
	ekf_pos(mImu.position + 2, mImu.velocity + 2, mImu.accel_w + 2,mUltra.distance,R,Q,dt);
//  mImu.velocity[1] +=  FILTER1ST_COEF(30,dt)*(mImu.velocity[2] - mImu.velocity[1]);
//	ANO_DT_Send_Sensor(mImu.velocity[1]*100,mImu.velocity[2]*100,mUltra.distance*100,mImu.accel_w[2]*1000,mImu.accel_norm*1000,0, 0,0,0);
}



#define ALT_WIN_SIZE		6



float win_alt[ALT_WIN_SIZE];
uint16_t index_alt=0;

float win_vz[ALT_WIN_SIZE];
uint16_t index_vz=0;

void alt_update_ultra()
{	
	float alt_pre;
	
	float dt;
	static uint32_t tPre = 0;
	uint32_t  tNow;
	static uint8_t first = 1;
	if (first){ tPre = micros(); first = 0; return; }
	
	if(mUltra.IsUltraUpdated == 1)
	{
		tNow = micros();
		dt = (tNow - tPre) / 1000000.0f;
		tPre = tNow;
		
		alt_pre = mImu.position[1];
		mImu.position[1] = filter_window(mUltra.distance,win_alt,&index_alt,ALT_WIN_SIZE);
		mImu.velocity[1] = filter_window((mImu.position[1] - alt_pre)/dt,win_vz,&index_vz,ALT_WIN_SIZE);
		
		//ANO_DT_Send_Sensor(mImu.position[1]*100,mImu.velocity[1]*100,mUltra.distance*100,mImu.accel_w[2]*1000,0,0, 0,0,0);
		mUltra.IsUltraUpdated = 0;
	}
	
}


