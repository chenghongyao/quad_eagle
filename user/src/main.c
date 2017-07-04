#include "sys.h"
#include "usart.h"
#include "string.h"
#include "key.h"


#include "route.h"
#include "csp.h"
#include "sbus.h"




//双缓冲
uint8_t eagle_buffer1[CAMERA_SIZE];//2.4k@160x120	
uint8_t eagle_buffer2[CAMERA_SIZE];//2.4k@160x120

//无线图像发送缓冲
uint8_t eagleRdioBuffer[CAMERA_SIZE];//2.4k@160x120	,用于无线异步发送

//解析结构
uint8_t eagle_route[CAMERA_H];



uint8_t radioUploadFinish = 1;		//异步发送完成
uint16_t route_start=60;					//循迹追踪起始位置
int16_t distance=0;								//离中心距离
float slope=0.0f;									//切线角度
uint8_t fLed = 0;




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
	
	beep_update();
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


void loop_main()
{
	//!!!加入超时重启
	if(meagle.status == EAGLE_FINISH)			//约26ms进入一次,处理程序最好再20ms以内
	{
		fLed = 1;
		meagle.status = EAGLE_IDLE;	
		eagle_startCapture();			//双缓冲,更新当前图像,同时启动采集下一幅图像 
		
		/**********图像处理**************/
		if(radioUploadFinish == 1	)//上一帧上传完成
		{
			memcpy(eagleRdioBuffer,meagle.image_use,CAMERA_SIZE);
			radioUploadFinish = 0;
		}
		
		//eagle_uploadImage(meagle.image_use);
		route_start = route_find(meagle.image_use,eagle_route,CAMERA_H-1,CAMERA_H,route_start);	
		distance =  route_getAverageDistance(eagle_route,0,5);	//第0到第10行,求线到中心距离
		slope = route_getSlope(eagle_route,0,10);								//求第0行到第10行的拟合斜率	
		csp_sendEagle(-distance,slope);
	}
	

	if(radioUploadFinish == 0)
	{
		//radioUploadFinish = eagle_uploadImageAnsy(eagleRdioBuffer);
	}
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
	wos_add(loop_50Hz,50);	//按键检测
	wos_add(loop_1Hz,1);		//flash 参数保存
	eagle_startCapture();		//	第一次启动
	wos_run();

}

