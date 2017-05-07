#include "sys.h"
#include "queue.h"
#include <string.h>




void queue_init(queue_t *q,uint8_t *buf,uint16_t len)
{
	q->pw=q->pr=q->pbuf=buf;
	q->pend = buf+len;
}


/******************************************************************************* 
���ܣ�д��һ������
������
q:
dat
����: д��״̬:
1 - д��ɹ�
0 - д��ʧ��(��������)
ʱ�䣺1/17/2017
ע�⣺ 
*******************************************************************************/  
uint8_t  queue_push(queue_t *q,uint8_t dat)
{
	uint8_t *ptemp;
	ptemp = q->pw + 1;
	if(ptemp == q->pend)ptemp = q->pbuf;
	if(ptemp != q->pr)
	{
		*q->pw = dat;
		q->pw = ptemp;
		return 1;
	}
	return 0;
}



uint8_t queue_get(queue_t *q)
{
	uint8_t dat;
	if(q->pr==q->pw)			//�ն���
		return 0; 
	else
	{
		dat = *q->pr;
		if(++q->pr == q->pend)q->pr = q->pbuf;
		return dat;
	}
}



uint16_t queue_putbuf(queue_t *q, uint8_t *buf, uint16_t len)
{
	uint8_t *pr = q->pr;
	uint8_t *pt = q->pw;

	uint16_t  vaillen,reallen = 0;
	uint16_t total = 0;

	while (len>0 && pr != pt)						//δд����δ����
	{
		vaillen = (pr > pt) ? (pr - pt) : (q->pend - pt); //���ο�д
		reallen = (vaillen < len) ? vaillen : len;		 //ʵ��д,ȡ��Сֵ

		memcpy(pt,buf,reallen);						//����
		//����
		buf += reallen;
		len -= reallen;
		total += reallen;
		pt += reallen;
		if (pt == q->pend)pt = q->pbuf;
	}
	q->pw = pt;
	return total;
}

/******************************************************************************* 
���ܣ���ȡ��������
������
q:���нṹ��ָ��
buf:����
len:��ȡ����
����: ʵ�ʶ�ȡ����
ʱ�䣺1/17/2017
ע�⣺ 
*******************************************************************************/ 
uint16_t queue_getbuf(queue_t *q, uint8_t *buf, uint16_t len)
{
	uint8_t *pw = q->pw;
	uint8_t *pt = q->pr;

	uint16_t  vaillen,reallen = 0;
	uint16_t total = 0;

	while (len>0 && pw != pt)						//δ������δ����
	{
		vaillen = (pw > pt) ? (pw - pt) : (q->pend - pt); //���οɶ�
		reallen = (vaillen < len) ? vaillen : len;		 //ʵ�ʶ�ȡ,ȡ��Сֵ

		memcpy(buf, pt, reallen);						//����
		//����
		buf += reallen;
		len -= reallen;
		total += reallen;
		pt += reallen;
		if (pt == q->pend)pt = q->pbuf;
	}
	q->pr = pt;
	return total;
}
/******************************************************************************* 
���ܣ���ȡһ������
������
q:���нṹ��ָ��
buf:����
len:��󳤶�,������\r\n
����: ��ȡ״̬
1 - ��ȡ���
0 - ��ȡδ���
ʱ�䣺1/17/2017
ע�⣺ 
һ��������\r\n��β
β���Զ��� \0
��һ�����ݴ�����󳤶�,ֻ��ȡǰ��������
*******************************************************************************/  
uint8_t queue_gets(queue_t *q, uint8_t *buf, uint16_t len)
{
	uint16_t reallen;
	uint8_t fgetr = 0;
	uint8_t *pw = q->pw;
	uint8_t *pt = q->pr;
	uint8_t restart = 0;

	while(pt != pw)				//��ȡ��������
	{
		if (*pt == '\r')fgetr = 1;
		else if (fgetr)
		{
			if (*pt == '\n')
			{
				if (restart)
				{
					if (pt != q->pbuf)
					{
						reallen = q->pend - q->pr;
						reallen = reallen < len ? reallen : len;
						memcpy(buf, q->pr, reallen);
						buf += reallen;
						len -= reallen;

						reallen = pt - 1 - q->pbuf;
						reallen = reallen < len ? reallen : len;
						memcpy(buf, q->pbuf, reallen);
					}
					else
					{
						reallen = q->pend - 1 - q->pr;
						reallen = reallen < len ? reallen : len;
						memcpy(buf, q->pr, reallen);
					}
					
				}
				else
				{
					reallen = pt - 1 - q->pr;
					reallen = reallen < len ? reallen : len;
					memcpy(buf, q->pr, reallen);
				}

				buf[reallen] = '\0';
				q->pr = pt + 1;
				if (q->pr == q->pend)q->pr = q->pbuf;
				return 1;
			}
			else
			{
				fgetr = 0;
			}
		}
		if (++pt==q->pend)		//���н�β
		{
			restart = 1;
			pt = q->pbuf;
		}
	} 
	return 0;
}


uint8_t queue_isempty(queue_t *q)
{
	if(q->pw==q->pr)return 1;
	return 0;
}



void queue_clear(queue_t *q)
{
	q->pw = q->pr;
}



uint16_t queue_length(queue_t *q)
{
	if(q->pw >= q->pr)return (q->pw - q->pr);
	else return (q->pend - q->pr + q->pw-q->pbuf);
}

