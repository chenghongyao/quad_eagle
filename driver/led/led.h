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
	SHINEALL,		//全部闪烁
	SHINENONE,	//不闪烁
	SHINEHEAD,	//闪头部
	SHINETAIL,	//闪尾部
	CYCLEALL,		//流水灯
	LIGHTHEAD,	//点亮头部
	LIGHTAIL,		//点亮尾部
	LIGHTALL,		//点亮全部
}LED_MODE;

extern uint8_t ledMode;

void led_init(void);	//初始化		
void led_set(u8 index,u8 sta);
void led_update(void);
void led_setMode(void);
#endif

















