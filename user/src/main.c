#include "sys.h"
#include "usart.h"
#include "string.h"
#include "key.h"


#include "route.h"
#include "csp.h"
#include "sbus.h"




//图像双缓冲
uint8_t eagle_buffer1[CAMERA_SIZE];//2.4k@160x120	
uint8_t eagle_buffer2[CAMERA_SIZE];//2.4k@160x120

//无线图像发送缓冲
uint8_t eagleRdioBuffer[CAMERA_SIZE];//2.4k@160x120	,用于无线异步发送

//解析结果
uint8_t eagle_route[CAMERA_H];
//uint8_t eagle_route2[CAMERA_H];


uint8_t radioUploadRouteFinish = 1;		//异步发送完成
uint8_t radioUploadFinish = 1;		//异步发送完成
uint16_t route_start=60;					//循迹追踪起始位置
int16_t distance=0;								//离中心距离
float slope=0.0f;									//切线斜率
uint8_t fLed = 0;

float route_der[CAMERA_H-2];		//标准差
float route_avr[CAMERA_H-4];		//平均值
float route_var[CAMERA_H-4];		//方差



//LED,BEEP
void loop_10Hz()
{
	if(fLed)
	{
		ledM = !ledM;
		fLed = 0;
	}
	else
	{
		ledM = 0;
	}
	
//	beep_update();
}


//KEY
void loop_50Hz()
{
	key_update();
	if(mKeys[KEY_UP].status == KEY_TRIG)
	{
		if(meagle.threshold == 0xff)
			beepMode = BEEP_SHORT_TWICE;
		else
		{
			if(eagle_setThreshold(meagle.threshold+1))
			{
				meagle.threshold ++;
				mTab.fSave = 1;
				debug("threshold=%x\r\n",meagle.threshold);
				beepMode = BEEP_SHORT;
			}	
		}
		
		key_cancel(KEY_UP);
	}
	else if(mKeys[KEY_DOWN].status == KEY_TRIG)
	{
		if(meagle.threshold == 0)
			beepMode = BEEP_SHORT_TWICE;
		else
		{
			if(eagle_setThreshold(meagle.threshold-1))
			{
				meagle.threshold --;
				mTab.fSave = 1;
				debug("threshold=%x\r\n",meagle.threshold);
				beepMode = BEEP_SHORT;
			}	
		}
		
		key_cancel(KEY_DOWN);
	}
	else if(mKeys[KEY_DOWN].status == KEY_LONGPRESS)
	{
		if(meagle.threshold == 0)
			beepMode = BEEP_SHORT_TWICE;
		else
		{
			if(eagle_setThreshold(meagle.threshold-1))
			{
				meagle.threshold --;
				mTab.fSave = 1;
				debug("threshold=%x\r\n",meagle.threshold);
				beepMode = BEEP_SHORT;
			}	
		}
		
	}
	else if(mKeys[KEY_UP].status == KEY_LONGPRESS)
	{
		if(meagle.threshold == 0xff)
			beepMode = BEEP_SHORT_TWICE;
		else
		{	
			if(eagle_setThreshold(meagle.threshold+1))
			{
				meagle.threshold ++;
				mTab.fSave = 1;
				debug("threshold=%x\r\n",meagle.threshold);
				beepMode = BEEP_SHORT;
			}	
		}
	}
}



//SAVE TABLE
void loop_1Hz()
{
	if(mTab.fSave)
	{
		table_save();
	}
}




#define VARIANCE_THRESHOLD 5   

void loop_main()
{
	static uint8_t t=0;
	static uint32_t tPre=0;
	uint16_t row;
	//!!!加入超时重启
	if(meagle.status == EAGLE_FINISH)			//约26.6ms进入一次,处理程序最好再20ms以内
	{
			fLed = 1;
		meagle.status = EAGLE_IDLE;	
		eagle_startCapture();								//双缓冲,更新当前图像,同时启动采集下一幅图像 
		/**********图像处理**************/
		if(radioUploadFinish == 1	)					//上一帧上传完成
		{
			memcpy(eagleRdioBuffer,meagle.image_use,CAMERA_SIZE);
			radioUploadFinish = 0;
		}
//		if(radioUploadRouteFinish == 1)					//上一帧上传完成
//		{
//			memcpy(eagle_route2,eagle_route,CAMERA_H);
//			radioUploadRouteFinish = 0;
//		}
		
		route_start = route_find2(meagle.image_use,eagle_route,0,CAMERA_H,route_start);	
		
		//计算方差
		for(row=0;row<CAMERA_H-2;row++)
		{
			route_der[row] = (eagle_route[row]+ eagle_route[row+2])/2.0f;
		}
		
		for(row=0;row<CAMERA_H-4;row++)
		{ 
			route_avr[row] = calculation_average(route_der[row],route_der[row+1],route_der[row+2]);
			route_var[row] = calculation_variance(route_der[row],route_der[row+1],route_der[row+2],route_avr[row]);
		}
		
		//突变点判断
		for(row=2;row<CAMERA_H-6;row++)
		{ 
			if(route_var[row]>VARIANCE_THRESHOLD)			//突变点
			{	
				if(calculation_average(route_avr[row],route_avr[row+1],route_avr[row+2])>calculation_average(route_avr[row],route_avr[row-1],route_avr[row-2]))
				{
					csp_sendRoute(ROUTE_RIGHT,row);
				}
				else
				{
					csp_sendRoute(ROUTE_LEFT,row);
				}
				break;
			}
		}

		distance =  route_getAverageDistance(eagle_route,0,5);		//第0到第10行,求线到中心距离
		slope = route_getSlope(eagle_route,0,10);								//求第0行到第10行的拟合斜率	
		csp_sendEagle(-distance,slope);
	}
	
	
//	if(t <10)
//	{
//		if(radioUploadFinish == 0)
//		{
//			radioUploadFinish = eagle_uploadImageAnsy(eagleRdioBuffer);
//			if(radioUploadFinish==1)t++;		
//		}
//	}
//	else if(t < 20)
//	{
//		if(radioUploadRouteFinish == 0)
//		{
//			radioUploadRouteFinish = eagle_uploadRouteAnsy(eagle_route2);
//			if(radioUploadRouteFinish==1)t++;		
//		}
//	}
//	else 
//	{
//		t = 0;
//	}

}



void route_init()
{
	uint16_t i;
	for(i=0;i<CAMERA_H;i++)
	{
		eagle_route[i]=20;
	}
}

int main(void)
{
	
	board_setup();
	debug("初始化...\r\n");
	table_load();	
	key_setup();
	nrf24l01_init();
	nrf24l01_setTxRxMode(NRF_TXEN);
	eagle_init(eagle_buffer1,eagle_buffer2);
	route_init();
	wos_init(loop_main);	
	wos_add(loop_10Hz,10);	//led,按键响应
	//wos_add(loop_50Hz,50);	//按键检测
	wos_add(loop_1Hz,1);		//flash 参数保存
	eagle_startCapture();		//	第一次启动
	wos_run();

}

