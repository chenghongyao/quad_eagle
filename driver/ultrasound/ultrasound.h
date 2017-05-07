#ifndef _ULTRASOUND_H
#define _ULTRASOUND_H
#include "sys.h"



#define ULTRASOUND_SPEED	340.0f		//�����ٶ�,m/s


typedef struct 
{	
	float distance;			//����,m
	uint8_t IsUltraUpdated;
}ultra_t;

extern ultra_t mUltra;
void ultrasound_trig(void);
void ultrasound_update(uint32_t dt_us);
#endif
