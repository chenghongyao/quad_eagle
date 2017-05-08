#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY_NUMBER		20
#define KEY_LONGPRESS_TIME	1000 //长按标志时间



typedef enum 
{
	KEY_NONE=0,
	KEY_PRESS,
	KEY_LONGPRESS,
	KEY_TRIG,
}key_status_t;


typedef enum
{
	KEY_ENT = 0,
	KEY_EXT,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,
	
	KEY_TRIM_CH1,	//通道微调键,但数字通道没有微调键
	KEY_TRIM_CH2,
	KEY_TRIM_CH3,
	KEY_TRIM_CH4,
	KEY_TRIM_CH6,
	KEY_TRIM_CH7,
	KEY_TRIM_CH8,
	KEY_TRIM_CH9,
	KEY_TRIM_CH10,
	KEY_TRIM_CH11,
	KEY_TRIM_CH12,
}key_name_t;


typedef struct 
{
	key_status_t status;
	uint8_t flagPress;
	uint32_t tStart;
}key_t;


extern key_t mKeys[KEY_NUMBER];
void key_test(void);
void key_setup(void);		
void key_update(void);						//定时调用 20ms
void key_cancel(key_name_t key);	//trig信号需要手动取消
#endif
