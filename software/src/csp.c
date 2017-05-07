#include "csp.h"
#include "nrf.h"
#include "usart.h"
#include "bytes.h"


//数据发送
static uint8_t csp_txbuf[32]={0xAA,0xAF};			//nrf发送,至少32byte
static uint8_t csp_txindex=4;

//数据接收
static uint8_t csp_rxbuf[32];//存放有效数据
static uint8_t csp_name,csp_len,csp_counter,csp_check=0,csp_step=0;


void csp_recvCallBack(uint8_t name,uint8_t *allbuf,uint8_t totallen)
{

}


//==============================================================
//0xaa 0xaf cmd len data sum
void csp_recvByte(uint8_t dat)
{
	switch(csp_step)
	{
		case 0:
			if(dat == 0xAA)
			{
				csp_check = dat;
				csp_rxbuf[0] = dat;
				csp_step++;
			}
			break;
		case 1:
			if(dat == 0xAF)
			{
				csp_check += dat;
				csp_rxbuf[1] = dat;
				csp_step++;
			}
			else csp_step = 0;
			break;
		case 2:			//功能字
			csp_name = dat;
			csp_check += dat;
			csp_rxbuf[2] = dat;
			csp_step++;	
			break;
		case 3:		//数据长度

			csp_rxbuf[3] = dat;		
			csp_counter = 0;	//有效数据计数清零
			csp_len = dat;
			csp_check += dat;
			if(dat == 0)csp_step+=2;		//没有有效数据
			else csp_step++;
			break;
		case 4:
			csp_rxbuf[4+csp_counter]=dat;
			csp_counter++;
			csp_check += dat;
			if(csp_counter==csp_len)csp_step++;
			break;
		case 5:
			if(csp_check == dat)		//和校验成功
			{
				csp_rxbuf[4+csp_counter] = dat;
				csp_recvCallBack(csp_name,csp_rxbuf,csp_len+5);//偏移4个字节为有效数据
			}
			csp_step=0;
			break;
		}
}
//


//==============================================

static void csp_setName(uint8_t name)
{
	csp_txindex=4;//引索复位
	csp_txbuf[2] = name;
}
static void csp_clearBuffer()
{
	csp_txindex = 4;
}




void csp_uploadUav(void)
{
	uint8_t i;
	uint8_t check=0;
	csp_txbuf[3] = (csp_txindex-4);
	for(i=0;i<csp_txindex;i++)
	{
		check += csp_txbuf[i];
	}
	csp_txbuf[csp_txindex++]=check;
	//nrf24l01_sendPacket(csp_txbuf,csp_txindex);
	nrf24l01_sendAckPacket(csp_txbuf,csp_txindex);
}


void csp_uploadPC(void)
{
	uint8_t i;
	uint8_t check=0;
	csp_txbuf[3] = (csp_txindex-4);
	for(i=0;i<csp_txindex;i++)
	{
		check += csp_txbuf[i];
	}
	csp_txbuf[csp_txindex++]=check;
	
	usart3_putbuf(csp_txbuf,csp_txindex);
	csp_clearBuffer();
}




void csp_put8(uint8_t dat)
{
	csp_txbuf[csp_txindex++] = dat;
}
void csp_put16(u16 dat)
{
	csp_txbuf[csp_txindex++] = dat;
	csp_txbuf[csp_txindex++] = dat>>8;
}

void csp_put32(u32 dat)
{
	csp_txbuf[csp_txindex++] = BYTE0(dat);
	csp_txbuf[csp_txindex++] = BYTE1(dat);
	csp_txbuf[csp_txindex++] = BYTE2(dat);;
	csp_txbuf[csp_txindex++] = BYTE3(dat);;
}
//


void csp_putFloat(float dat)
{
	csp_txbuf[csp_txindex++] = BYTE0(dat);
	csp_txbuf[csp_txindex++] = BYTE1(dat);
	csp_txbuf[csp_txindex++] = BYTE2(dat);
	csp_txbuf[csp_txindex++] = BYTE3(dat);
}
void csp_putBuf(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		csp_txbuf[csp_txindex++] = *buf++;
	}
}


void csp_sendCCD(uint8_t start,uint8_t mid,uint8_t end)
{
	csp_setName(CSP_CCD_DATA);
	csp_put8(start);
	csp_put8(mid);
	csp_put8(end);
	//csp_uploadUav();
	csp_uploadPC();
	
}

void csp_sendCCDImage(uint8_t index,uint8_t *buf)
{
	csp_setName(CSP_CCD_IMAGE);
	csp_put8(index);
	csp_putBuf(buf,16);
	csp_uploadUav();
}





