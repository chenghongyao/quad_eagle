#include "table.h"
#include "sys.h"

#include "controller.h"
#include "cmath.h"

table_t mTab;


void table_load()
{
	flash_read(STM32_FLASH_LASTPAGE,(u16*)&mTab,sizeof(mTab)/2);
	
	if(mTab.TableInited != 'Y')
	{
		/*PID*/
		mTab.RollRate_Kp = 1.169f;
		mTab.RollRate_Ki = 0.090f;
		mTab.RollRate_Kd = 0.114f;
		
		mTab.RollAngle_Kp = 4.172f;
		mTab.RollAngle_Ki = 0.072f;
		mTab.RollAngle_Kd = 0.00f;
		
//		mTab.RollRate_Kp = 0.0f;
//		mTab.RollRate_Ki = 0.0f;
//		mTab.RollRate_Kd = 0.0f;
//		
//		mTab.RollAngle_Kp = 0.0f;
//		mTab.RollAngle_Ki = 0.0f;
//		mTab.RollAngle_Kd = 0.00f;
		
		mTab.PitchRate_Kp = mTab.RollRate_Kp;
		mTab.PitchRate_Ki = mTab.RollRate_Ki;
		mTab.PitchRate_Kd = mTab.RollRate_Kd;
		
		mTab.YawRate_Kp = mTab.RollRate_Kp;
		mTab.YawRate_Ki = mTab.RollRate_Ki;
		mTab.YawRate_Kd = mTab.RollRate_Kd;
		
		
		mTab.PitchAngle_Kp = mTab.RollAngle_Kp;
		mTab.PitchAngle_Ki = mTab.RollAngle_Ki;
		mTab.PitchAngle_Kd = mTab.RollAngle_Kd;
		
		mTab.YawAngle_Kp = mTab.RollAngle_Kp;
		mTab.YawAngle_Ki = mTab.RollAngle_Ki;
		mTab.YawAngle_Kd = mTab.RollAngle_Kd;
		
		
		mTab.AltRate_Kp = 0.0f;
		mTab.AltRate_Ki = 0.0f;
		mTab.AltRate_Kd = 0.0f;
		
		mTab.Alt_Kp = 0.0f;
		mTab.Alt_Ki = 0.0f;
		mTab.Alt_Kd = 0.0f;
		
		/*´«¸ÐÆ÷*/		
		mTab.roll_corr = DEG2RAD(-1.9f);//mine
		mTab.pitch_corr = DEG2RAD(0.7f);

		
		mTab.roll_zero = 0.0f;
		mTab.pitch_zero = 0.0f;
		
		//Ð´Èëflash
		flash_write(STM32_FLASH_LASTPAGE,(u16*)&mTab,sizeof(mTab)/2);
	}
}


void table_save()
{
	/*PID*/
	mTab.RollRate_Kp = pidRollRate.kp;
	mTab.RollRate_Ki = pidRollRate.ki;
	mTab.RollRate_Kd = pidRollRate.kd;

	
	mTab.PitchRate_Kp  = pidPitchRate.kp;
	mTab.PitchRate_Ki  = pidPitchRate.ki;
	mTab.PitchRate_Kd = pidPitchRate.kd ;

	
	mTab.YawRate_Kp = pidYawRate.kp;
	mTab.YawRate_Ki  = pidYawRate.ki;
	mTab.YawRate_Kd  = pidYawRate.kd;

	
	mTab.RollAngle_Kp  = pidRollAngle.kp;
	mTab.RollAngle_Ki  = pidRollAngle.ki;
	mTab.RollAngle_Kd  = pidRollAngle.kd;

	
	mTab.PitchAngle_Kp  = pidPitchAngle.kp;
	mTab.PitchAngle_Ki  = pidPitchAngle.ki;
	mTab.PitchAngle_Kd  = pidPitchAngle.kd;


	mTab.YawAngle_Kp  = pidYawAngle.kp;
	mTab.YawAngle_Ki  = pidYawAngle.ki;
	mTab.YawAngle_Kd  = pidYawAngle.kd;

	
	
	mTab.AltRate_Kp = pidAltRate.kp;
	mTab.AltRate_Ki = pidAltRate.ki;
	mTab.AltRate_Kd = pidAltRate.kd;

	
	mTab.Alt_Kp = pidAlt.kp;
	mTab.Alt_Ki = pidAlt.ki;
	mTab.Alt_Kd = pidAlt.kd;
	
	flash_write(STM32_FLASH_LASTPAGE,(u16*)&mTab,sizeof(mTab)/2);
}
