#include "sys.h"
#include "usart.h"
#include "string.h"
#include "key.h"


#include "route.h"
#include "csp.h"

//LED,BEEP
void loop_10Hz()
{
	ledM = !ledM;
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



uint8_t eagle_buffer1[CAMERA_SIZE];//2.4k@160x120
uint8_t eagle_buffer2[CAMERA_SIZE];//2.4k@160x120
uint8_t eagleRdioBuffer[CAMERA_SIZE];//2.4k@160x120	,用于无线异步发送
uint8_t eagle_route[CAMERA_H];
uint8_t eagle_line[CAMERA_W];

uint8_t radioUploadFinish = 1;//异步发送完成
uint16_t route_start=60;
int16_t distance=0;							//离中心距离
float angle=0.0f;									//切线角度

void loop_main()
{
//	uint16_t i;
//	static uint32_t tPre;
	//!!!加入超时重启
	if(meagle.status == EAGLE_FINISH)			//约26ms进入一次,处理程序最好再20ms以内
	{
		
		meagle.status = EAGLE_IDLE;	
		eagle_startCapture();			//双缓冲,更新当前图像,同时启动采集下一幅图像 
		
//		printf("%d\r\n",micros()-tPre);		//显示调用时间
//		tPre = micros();
		
		/**********图像处理**************/
		if(radioUploadFinish == 1)//上一帧上传完成
		{
			memcpy(eagleRdioBuffer,meagle.image_use,CAMERA_SIZE);
			radioUploadFinish = 0;
		}
		route_start = route_find(meagle.image_use,eagle_route,CAMERA_H-1,CAMERA_H,route_start);	
		distance =  route_getAverageDistance(eagle_route,0,5);	//第0到第10行,求线到中心距离
		angle = route_getAngle(eagle_route,0,10);								//求第0行到第10行的拟合斜率	
		csp_sendEagle(distance,angle);
		//eagle_uploadImage(meagle.image_use);		//图像上传
	}
	if(radioUploadFinish == 0)
	{
		//radioUploadFinish = eagle_uploadImageAnsy(eagleRdioBuffer);
		//radioUploadFinish = eagle_uploadRouteAnsy(eagle_route);
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
	eagle_startCapture();			//第一次启动
	wos_run();

}




//void bufferinit()
//{
//	uint16_t i,j;
//	volatile uint8_t *p = eagle_buffer2;
//	for(i=0;i<CAMERA_H;i++)
//	{
//		for(j=0;j<7;j++)
//		{
//			*p++ = 0;
//		}
//					
//		for(j=0;j<6;j++)
//		{
//			*p++ = 0xff;
//		}
//		for(j=0;j<7;j++)
//		{
//			*p++ = 0;
//		}
//	}
//} 

