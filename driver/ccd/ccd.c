#include "ccd.h"
#include "sys.h"


#include "cmath.h"
#define	CCD_PUTC(_c)	H_USART_PUT8_F(1,_c)




ccd_t mccd;

//底层驱动,读取8位ADC,0-255
static uint8_t ccd_getAdc()
{
	uint16_t val;
	val = adc1_getAdc(3);
	return (uint8_t)(val>>4);
}


//计算128个点的平均值
uint8_t ccd_calcImageAverage()
{
	uint8_t i;
	uint16_t sum=0;
	uint8_t *pimage = mccd.image;
	
	for(i=0;i<128;i++)
	{
		sum+=*pimage++;
	}
	return (uint8_t)(sum/128);
}



//计算曝光时间
void ccd_updateIntegrationTime()
{
	uint8_t pixel_avr;
	int16_t err;
	
	pixel_avr = ccd_calcImageAverage();
	err = CCD_PIXEL_TAR - (int16_t)pixel_avr;				//电压误差
	
	
	//调整曝光值
	if(err<-CCD_MAX_ERR)mccd.intTime--;
	else if(err>CCD_MAX_ERR)mccd.intTime++;
	//曝光时间限幅
	if(mccd.intTime<5)mccd.intTime = 5;
	else if(mccd.intTime>45)mccd.intTime = 45;
	
}



//读取图像数据
void ccd_update()
{
	uint8_t i;
	uint8_t *pimage=mccd.image;
	
	CCD_SI = 1;
	delay_us(1);		
	CCD_CLK = 1;	
	CCD_SI = 0;
	delay_us(1);	
	for(i=0;i<128;i++)
	{
		CCD_CLK = 0;
		*pimage++ = ccd_getAdc();
		CCD_CLK = 1;
		delay_us(1);
	}
	CCD_CLK = 0;
}


//启动曝光,运行ccd_startIntegration()函数到运行ccd_readImage()函数的时间为曝光时间
void ccd_startIntegration()
{
	uint8_t i;
	CCD_SI = 1;
	delay_us(1);		
	CCD_CLK = 1;	//第一个CLK
	CCD_SI = 0;
	delay_us(1);	
	CCD_CLK = 0;
	for(i=1;i<129;i++)	//第二到第129个CLK
	{
		delay_us(1);
		CCD_CLK = 1;
		delay_us(1);
		CCD_CLK = 0;
	}
}
void ccd_readImage()
{
	uint8_t i;   
	uint8_t *pimage=mccd.image;
	
	//采集上次曝光的数据
	CCD_SI = 1;	
	delay_us(1);		
	CCD_CLK = 1;							//第一个CLK
	CCD_SI = 0;
	*pimage++ = ccd_getAdc();	//采集第一个点
	CCD_CLK = 0;
	
	//采集第2到第128个点
	for(i=1;i<128;i++)	
	{
		delay_us(1);
		CCD_CLK = 1;
		*pimage++ = ccd_getAdc();
		CCD_CLK = 0;
	}
	//发送第129个CLK
	delay_us(1);
	CCD_CLK = 1;
	delay_us(1);
	CCD_CLK = 0;
	delay_us(1);
}



#define CCD_LINE_DIS	30
#define CCD_LINE_NUM	5
#define CCD_DIS_CHANGE_MAX	20
//void ccd_fineLine()
//{
//	uint8_t i;
//	uint8_t lines[CCD_LINE_NUM];	//最多寻找5条线
//	uint8_t t=0;
//	s16 dis;
//	uint8_t fline = 0;
//	
//	
//	//寻找白线->黑线交界线
//	for(i=0;i<127;i++)		
//	{
//		dis = (s16)mccd.image[i]-(s16)mccd.image[i+1];
//		if(dis>CCD_LINE_DIS && fline == 0)
//		{
//			fline = 1;
//			lines[t++] = i;
//			if(t == CCD_LINE_NUM)break;
//		}
//		else if(dis<0)
//		{
//			fline = 0;
//		}
//	}
//	
//	
//	if(t >= 2)	//至少获取两条线,丢失则使用上一次的结果
//	{
//		
//		for(i=0;i<t-1;i++)		//对找到的线进行判断
//		{
//			if(mccd.iLeft == 255)		//位置未赋值
//			{
//				if(t == 2)		//两条线,姑且相信他是正确的
//				{
//					mccd.iLeft = lines[0];
//					mccd.iRight = lines[1];
//				}
//			}
//			else		//与上一时刻的位置比较,起始线误差不能过大
//			{
//				 if (ABS(lines[i] - mccd.iLeft) < 20	)	
//					{
//							mccd.iLeft = lines[i];
//							mccd.iRight = lines[i + 1];
//							break;                      
//					}
//			}
//		}
//		mccd.iMid = (mccd.iLeft+mccd.iRight)/2;
//	}
//}


void ccd_fineLine()
{
	uint8_t i;
	uint8_t t=0;
	s16 dis;
	uint8_t fline = 0;
	
	for(i=0;i<5;i++)
	{
		mccd.lines[i] = 255;
	}
	
	//寻找白线->黑线交界线
	for(i=10;i<115;i++)		
	{
		dis = (s16)mccd.image[i]-(s16)mccd.image[i+3];
		if(dis>CCD_LINE_DIS && fline == 0)		//防止多次进入
		{
			fline = 1;
			mccd.lines[t++] = i;
			if(t == CCD_LINE_NUM)break;
		}
	}
	
	
	if(t >= 2)	//至少获取两条线,丢失则使用上一次的结果
	{
			mccd.iLeft = mccd.lines[0];
			mccd.iRight = mccd.lines[1];
			mccd.iMid = (mccd.iLeft+mccd.iRight)/2;
	}
	


}

void ccd_init()
{
	uint8_t i;
	uint8_t *p_image = mccd.image;
	//清除图像数据
	for(i=0;i<128;i++)
	{
		*p_image++ = 		0;
	}
	mccd.msCounter = 0;
	mccd.intTime = 40;
	
	
	mccd.iLeft = 255;
	mccd.iRight = 255;
	mccd.iMid = 255;
}





//一个字节转16进制ASCII
//static void ccd_sendHex(uint8_t hex)
//{
//	uint8_t temp;
//	//高4位
//	temp = hex>>4;
//	if(temp>9)temp+=7;
//	CCD_PUTC(temp+'0');
//	//低4位
//	temp = hex&0x0F;
//	if(temp>9)temp+=7;
//	CCD_PUTC(temp+'0');
//}
////发送数据到上位机
//void ccd_sendImage()
//{
//	uint8_t i;
//	uint8_t crc=0;
//	uint8_t *p_image = mccd.image;
//	
//	CCD_PUTC('*');
//	CCD_PUTC('L');
//	CCD_PUTC('D');
//	
//	ccd_sendHex(0);
//	ccd_sendHex(0);
//	ccd_sendHex(0);
//	ccd_sendHex(0);
//	
//	for(i=0;i<128;i++)
//	{
//		ccd_sendHex(*p_image++);
//	}
//	ccd_sendHex(crc);
//	CCD_PUTC('#');
//}

