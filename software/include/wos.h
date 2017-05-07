#ifndef _WOS_H
#define _WOS_H
#include "sys.h"


/////////////////////////////////////////////////////
//ע�ʹ˺�ر�ʱ���¼,
////////////////////////////////////////////////////
//#define WOS_USE_TIMERECORD	


typedef void(*pOSFunc_t)(void);


int8_t  wos_add(pOSFunc_t pfunc, uint16_t frequency);
void wos_remove(uint8_t id);
void wos_run(void);
void wos_update(void);
void wos_init(pOSFunc_t mainfun);
	
#ifdef WOS_USE_TIMERECORD
uint32_t  wos_period(uint8_t  id);		//��ȡ������������
uint32_t wos_time(uint8_t id);			//��ȡ����ִ��ʱ��
#endif


#endif
