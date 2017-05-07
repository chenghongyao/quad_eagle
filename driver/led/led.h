#ifndef __LED_H
#define __LED_H	 
#include "sys.h"




#define LED_M1	1
#define LED_M2	2
#define LED_M3	4
#define LED_M4	8


#define LED_HEAD	LED_M4|LED_M3
#define LED_TAIL	LED_M1|LED_M2
#define LED_ALL		LED_HEAD|LED_TAIL


typedef enum
{
	SHINEALL,		//ȫ����˸
	SHINENONE,	//����˸
	SHINEHEAD,	//��ͷ��
	SHINETAIL,	//��β��
	CYCLEALL,		//��ˮ��
	LIGHTHEAD,	//����ͷ��
	LIGHTAIL,		//����β��
	LIGHTALL,		//����ȫ��
}LED_MODE;

extern uint8_t ledMode;

void led_init(void);	//��ʼ��		
void led_set(u8 index,u8 sta);
void led_update(void);
void led_setMode(void);
#endif

















