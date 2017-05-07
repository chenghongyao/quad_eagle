#ifndef _TABLE_H
#define _TABLE_H
#include "sys.h"

typedef struct
{
	uint8_t TableInited;
	
	/*传感器与重力平面的误差*/
	float roll_corr;
	float pitch_corr;
	
	/*倾斜修正*/
	float roll_zero;		
	float pitch_zero;
	
	
	/*PID*/
	float RollRate_Kp;
	float RollRate_Ki;
	float RollRate_Kd;
	
	float PitchRate_Kp;
	float PitchRate_Ki;
	float PitchRate_Kd;
	
	float YawRate_Kp;
	float YawRate_Ki;
	float YawRate_Kd;
	
	float RollAngle_Kp;
	float RollAngle_Ki;
	float RollAngle_Kd;
	
	float PitchAngle_Kp;
	float PitchAngle_Ki;
	float PitchAngle_Kd;
	
	float YawAngle_Kp;
	float YawAngle_Ki;
	float YawAngle_Kd;

	float Alt_Kp;
	float Alt_Ki;
	float Alt_Kd;

	float AltRate_Kp;
	float AltRate_Ki;
	float AltRate_Kd;
	
	/**/
}table_t;

extern table_t mTab;
void table_load(void);
void table_save(void);

#endif
