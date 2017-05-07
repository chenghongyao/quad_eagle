#ifndef _QUEUE__H
#define _QUEUE__H
#include "sys.h"

typedef struct
{
	uint8_t *pbuf;
	uint8_t *pw;
	uint8_t *pr;	
	uint8_t *pend;
}queue_t;


void queue_init(queue_t *q,uint8_t *buf,u16 len);			//用缓存和长度初始化队列
uint8_t  queue_push(queue_t *q,uint8_t dat);					//存入一个数据	
uint8_t queue_get(queue_t *q);												//读取一个数据
u16 queue_length(queue_t *q);													//获取当前长度
void queue_clear(queue_t *q);													//清除所有数据
uint16_t queue_putbuf(queue_t *q, uint8_t *buf, uint16_t len);	//写入指定长度的数据
uint16_t queue_getbuf(queue_t *q,uint8_t *buf,uint16_t len);	//读取指定长度的数据
uint8_t queue_gets(queue_t *q, uint8_t *buf, uint16_t len);	//读取一行,以\r\n结尾
#endif
