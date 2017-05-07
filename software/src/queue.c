#include "sys.h"
#include "queue.h"
#include <string.h>




void queue_init(queue_t *q,uint8_t *buf,uint16_t len)
{
	q->pw=q->pr=q->pbuf=buf;
	q->pend = buf+len;
}


/******************************************************************************* 
功能：写入一个数据
参数：
q:
dat
返回: 写入状态:
1 - 写入成功
0 - 写入失败(队列已满)
时间：1/17/2017
注意： 
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
	if(q->pr==q->pw)			//空队列
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

	while (len>0 && pr != pt)						//未写完且未结束
	{
		vaillen = (pr > pt) ? (pr - pt) : (q->pend - pt); //本次可写
		reallen = (vaillen < len) ? vaillen : len;		 //实际写,取最小值

		memcpy(pt,buf,reallen);						//复制
		//更新
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
功能：读取队列内容
参数：
q:队列结构体指针
buf:缓存
len:读取长度
返回: 实际读取长度
时间：1/17/2017
注意： 
*******************************************************************************/ 
uint16_t queue_getbuf(queue_t *q, uint8_t *buf, uint16_t len)
{
	uint8_t *pw = q->pw;
	uint8_t *pt = q->pr;

	uint16_t  vaillen,reallen = 0;
	uint16_t total = 0;

	while (len>0 && pw != pt)						//未读完且未结束
	{
		vaillen = (pw > pt) ? (pw - pt) : (q->pend - pt); //本次可读
		reallen = (vaillen < len) ? vaillen : len;		 //实际读取,取最小值

		memcpy(buf, pt, reallen);						//复制
		//更新
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
功能：读取一行数据
参数：
q:队列结构体指针
buf:缓存
len:最大长度,不包括\r\n
返回: 读取状态
1 - 读取完成
0 - 读取未完成
时间：1/17/2017
注意： 
一行数据以\r\n结尾
尾部自动加 \0
当一行数据大于最大长度,只截取前部分数据
*******************************************************************************/  
uint8_t queue_gets(queue_t *q, uint8_t *buf, uint16_t len)
{
	uint16_t reallen;
	uint8_t fgetr = 0;
	uint8_t *pw = q->pw;
	uint8_t *pt = q->pr;
	uint8_t restart = 0;

	while(pt != pw)				//读取所有数据
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
		if (++pt==q->pend)		//本行结尾
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

