#include "led.h"
#include "imu.h"
#include "flight.h"



uint8_t ledMode=SHINEALL;

void led_set(uint8_t index,uint8_t sta)
{
	ledM = !sta;
//	if(index&LED_M1)ledM1 = sta;
//	if(index&LED_M2)ledM2 = sta;
//	if(index&LED_M3)ledM3 = sta;
//	if(index&LED_M4)ledM4 = sta;
}

void led_update()
{
	static uint8_t ledstatus = 0;
	static uint8_t ledbuf = LED_M1;

//	static uint8_t counter=0;
//	if (++counter < 10)return;		//调节控制频率
//	counter = 0;
	led_setMode();
	switch(ledMode)
	{
		case SHINEALL:
			led_set(LED_ALL,ledstatus);
			break;
		case SHINENONE:
			led_set(LED_ALL,0);
			break;
		case SHINEHEAD:
			led_set(LED_HEAD,ledstatus);
			led_set(LED_TAIL,0);
			break;
		case SHINETAIL:
			led_set(LED_HEAD,0);
			led_set(LED_TAIL,ledstatus);
			break;
		case CYCLEALL:
			led_set(LED_ALL,0);
			led_set(ledbuf,1);			
			if(ledbuf==LED_M4)
				ledbuf = LED_M1;
			else 
				ledbuf<<=1;
			break;
		case LIGHTHEAD:
			led_set(LED_HEAD,1);
			led_set(LED_TAIL,0);
		break;
		case LIGHTAIL:
			led_set(LED_HEAD,0);
			led_set(LED_TAIL,1);
		break;
		case LIGHTALL:
			led_set(LED_ALL,1);
		break;
		default:
			break;
	}
	ledstatus = !ledstatus;
}



/////////////////////////////////////////////////////
//校准中->CYCLEALL
//丢失信号->LIGHTHEAD
//正常->SHINEHEAD
//越往下优先级越高
////////////////////////////////////////////////////
void led_setMode()
{
	//if(mFlyFlag.lowBatary)ledMode = SHINEALL;
	//if(mFlyFlag.flyEnable)ledMode = SHINEALL;	
	
	if(mImu.hasIMUReady == 0)
	{
		ledMode = SHINEALL;
	}
	else if(mFlight.flyEnable)
	{
		ledMode = SHINEALL;
	}
	else
	{
		ledMode = LIGHTALL;
	}
}
