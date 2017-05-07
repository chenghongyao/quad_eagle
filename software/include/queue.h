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


void queue_init(queue_t *q,uint8_t *buf,u16 len);			//�û���ͳ��ȳ�ʼ������
uint8_t  queue_push(queue_t *q,uint8_t dat);					//����һ������	
uint8_t queue_get(queue_t *q);												//��ȡһ������
u16 queue_length(queue_t *q);													//��ȡ��ǰ����
void queue_clear(queue_t *q);													//�����������
uint16_t queue_putbuf(queue_t *q, uint8_t *buf, uint16_t len);	//д��ָ�����ȵ�����
uint16_t queue_getbuf(queue_t *q,uint8_t *buf,uint16_t len);	//��ȡָ�����ȵ�����
uint8_t queue_gets(queue_t *q, uint8_t *buf, uint16_t len);	//��ȡһ��,��\r\n��β
#endif
