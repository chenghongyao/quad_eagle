#include "sys.h"
#include "usart.h"


#include "key.h"

void loop_10Hz()
{
	ledM = !ledM;
	beep_update();
	
}
void loop_50Hz()
{
	key_update();
	if(mKeys[KEY_UP].status == KEY_TRIG)
	{
		meagle.threshold ++;
		if(eagle_setThreshold(meagle.threshold))
		{
			mTab.fSave = 1;
			debug("threshold=%x\r\n",meagle.threshold);
			if(meagle.threshold == 0xff)beepMode = BEEP_SHORT_TWICE;
			else beepMode = BEEP_SHORT;
		}	
		key_cancel(KEY_UP);
	}
	else if(mKeys[KEY_DOWN].status == KEY_TRIG)
	{
		meagle.threshold --;
		if(eagle_setThreshold(meagle.threshold))
		{
			mTab.fSave = 1;
			debug("threshold=%x\r\n",meagle.threshold);
			if(meagle.threshold == 0xff)beepMode = BEEP_SHORT_TWICE;
			else beepMode = BEEP_SHORT;
			
			beepMode = BEEP_SHORT;
		}	
		key_cancel(KEY_DOWN);
	}
	else if(mKeys[KEY_DOWN].status == KEY_LONGPRESS)
	{
			meagle.threshold --;
		if(eagle_setThreshold(meagle.threshold))
		{
			mTab.fSave = 1;
			debug("threshold=%x\r\n",meagle.threshold);
			if(meagle.threshold == 0xff)beepMode = BEEP_SHORT_TWICE;
			else beepMode = BEEP_SHORT;
			
			beepMode = BEEP_SHORT;
		}	
	}
	else if(mKeys[KEY_UP].status == KEY_LONGPRESS)
	{
		meagle.threshold ++;
		if(eagle_setThreshold(meagle.threshold))
		{
			mTab.fSave = 1;
			debug("threshold=%x\r\n",meagle.threshold);
			if(meagle.threshold == 0xff)beepMode = BEEP_SHORT_TWICE;
			else beepMode = BEEP_SHORT;
		}	
		key_cancel(KEY_UP);
	}
}
void loop_1Hz()
{
	if(mTab.fSave)
	{
		table_save();
	}
}

uint8_t eagle_buffer1[CAMERA_SIZE];
uint8_t eagle_buffer2[CAMERA_SIZE];
void bufferinit()
{
	uint16_t i,j;
	volatile uint8_t *p = eagle_buffer2;
	for(i=0;i<CAMERA_H;i++)
	{
		for(j=0;j<7;j++)
		{
			*p++ = 0;
		}
		
		for(j=0;j<6;j++)
		{
			*p++ = 0;
		}
		for(j=0;j<7;j++)
		{
			*p++ = 0;
		}
	}
} 


void loop_main()
{
	static uint32_t tPre=0;
	uint32_t tNow;
	if(meagle.hasUpdate == 1)			//约50ms进入一次,处理程序最好再50ms以内
	{
		meagle.hasUpdate = 0;	
		eagle_startCapture();			//使用双缓冲,更新当前图像,同时启动采集下一幅图像

		eagle_uploadImage();		//图像上传
	
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
	bufferinit();
	eagle_uploadImage();
	wos_init(loop_main);
	
//	wos_add(loop_10Hz,10);	//led,按键响应
//	wos_add(loop_50Hz,50);	//按键检测
//	wos_add(loop_1Hz,1);		//flash 参数保存
	eagle_startCapture();			//第一次启动
	wos_run();

}


