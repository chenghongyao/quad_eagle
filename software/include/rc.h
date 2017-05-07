#ifndef _RC_H
#define _RC_H

#include "sys.h"


#define RC_DB		50

#define RC_MAXLINE	2000-100
#define RC_MINLINE	1000+100

#define RC_ANGLEMAX	 20		//��
#define RC_YAWRATEMAX 50	//��/s

#define RC_TIMEOUT_ENABLE	1			//ʹ��ң�س�ʱ����	
#define RC_TIMEOUT	500				//ң���źų�ʱʱ��,ms


enum 
{
	CH_THRO = 0,
	CH_YAWRATE = 1,
	CH_ROLL = 2,
	CH_PITCH = 3,
	CH_GEAR = 4,
	CH_6 = 5,
};

typedef struct 
{
	uint16_t channel[9];
	float thro;
	float roll;
	float pitch;
	float yaw_rate;
}rc_t;

extern rc_t mRC;


void rc_init(void);
void rc_update(void);
float rc_dbScaleLinear(float x, float x_end, float deadband);
void rc_dealRCData(void);
#endif
