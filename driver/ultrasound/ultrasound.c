#include "ultrasound.h"
#include "imu.h"
#include "sys.h"
#include "filter.h"

ultra_t mUltra;


void ultrasound_trig()
{
	ULTRA_TRIG = 1;
	delay_us(40);
	ULTRA_TRIG = 0;
}


void ultrasound_update(uint32_t dt_us)
{
	float dis = (ULTRASOUND_SPEED / (2.0*1000000.0f))*dt_us;
	if(dis>2.5f)dis = 0.0f;				//³¬¹ý²âÁ¿·¶Î§
	
	mUltra.distance  += FILTER1ST_COEF(5,0.1)*(dis -  mUltra.distance);
	mUltra.IsUltraUpdated = 1;
}
