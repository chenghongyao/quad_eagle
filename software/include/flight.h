#ifndef _FLIGHT_H
#define _FLIGHT_H
#include "sys.h"
typedef enum {
	//根据ANO上位机定义
	FLMODE_NONE = 0,			//无
	FLMODE_ATTITUDE = 1,		//姿态
	FLMODE_HOLD_ALT = 2,		//定高
	FLMODE_HOLD_POS = 3,		//定点
	FLMODE_ROUTE = 11,		//航线
	FLMODE_LANDING = 20,		//降落
	FLMODE_RETURN = 21,		//返航
	//自定义

}flight_mode_t;


typedef struct 
{
	flight_mode_t flyMode;
	//
	uint8_t flyEnable;
	uint8_t	lostRadio;
	uint8_t recvRCData;


	uint8_t saveTable;
}flight_t;


#define FL_ANGEL_MAX 30.0f

extern flight_t mFlight;
void flight_init(void);
void flight_safeControl(void);
void flight_lock(void);
void flight_unlock(void);

#endif
