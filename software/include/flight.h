#ifndef _FLIGHT_H
#define _FLIGHT_H
#include "sys.h"
typedef enum {
	//����ANO��λ������
	FLMODE_NONE = 0,			//��
	FLMODE_ATTITUDE = 1,		//��̬
	FLMODE_HOLD_ALT = 2,		//����
	FLMODE_HOLD_POS = 3,		//����
	FLMODE_ROUTE = 11,		//����
	FLMODE_LANDING = 20,		//����
	FLMODE_RETURN = 21,		//����
	//�Զ���

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
