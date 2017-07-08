#include "sys.h"
#include "usart.h"
#include "string.h"
#include "key.h"


#include "route.h"
#include "csp.h"
#include "sbus.h"




//ͼ��˫����
uint8_t eagle_buffer1[CAMERA_SIZE];//2.4k@160x120	
uint8_t eagle_buffer2[CAMERA_SIZE];//2.4k@160x120

//����ͼ���ͻ���
uint8_t eagleRdioBuffer[CAMERA_SIZE];//2.4k@160x120	,���������첽����

//�������
uint8_t eagle_route[CAMERA_H];
//uint8_t eagle_route2[CAMERA_H];


uint8_t radioUploadRouteFinish = 1;		//�첽�������
uint8_t radioUploadFinish = 1;		//�첽�������
uint16_t route_start=60;					//ѭ��׷����ʼλ��
int16_t distance=0;								//�����ľ���
float slope=0.0f;									//����б��
uint8_t fLed = 0;

float route_der[CAMERA_H-2];		//��׼��
float route_avr[CAMERA_H-4];		//ƽ��ֵ
float route_var[CAMERA_H-4];		//����



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
	//!!!���볬ʱ����
	if(meagle.status == EAGLE_FINISH)			//Լ26.6ms����һ��,������������20ms����
	{
			fLed = 1;
		meagle.status = EAGLE_IDLE;	
		eagle_startCapture();								//˫����,���µ�ǰͼ��,ͬʱ�����ɼ���һ��ͼ�� 
		/**********ͼ����**************/
		if(radioUploadFinish == 1	)					//��һ֡�ϴ����
		{
			memcpy(eagleRdioBuffer,meagle.image_use,CAMERA_SIZE);
			radioUploadFinish = 0;
		}
//		if(radioUploadRouteFinish == 1)					//��һ֡�ϴ����
//		{
//			memcpy(eagle_route2,eagle_route,CAMERA_H);
//			radioUploadRouteFinish = 0;
//		}
		
		route_start = route_find2(meagle.image_use,eagle_route,0,CAMERA_H,route_start);	
		
		//���㷽��
		for(row=0;row<CAMERA_H-2;row++)
		{
			route_der[row] = (eagle_route[row]+ eagle_route[row+2])/2.0f;
		}
		
		for(row=0;row<CAMERA_H-4;row++)
		{ 
			route_avr[row] = calculation_average(route_der[row],route_der[row+1],route_der[row+2]);
			route_var[row] = calculation_variance(route_der[row],route_der[row+1],route_der[row+2],route_avr[row]);
		}
		
		//ͻ����ж�
		for(row=2;row<CAMERA_H-6;row++)
		{ 
			if(route_var[row]>VARIANCE_THRESHOLD)			//ͻ���
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

		distance =  route_getAverageDistance(eagle_route,0,5);		//��0����10��,���ߵ����ľ���
		slope = route_getSlope(eagle_route,0,10);								//���0�е���10�е����б��	
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
	debug("��ʼ��...\r\n");
	table_load();	
	key_setup();
	nrf24l01_init();
	nrf24l01_setTxRxMode(NRF_TXEN);
	eagle_init(eagle_buffer1,eagle_buffer2);
	route_init();
	wos_init(loop_main);	
	wos_add(loop_10Hz,10);	//led,������Ӧ
	//wos_add(loop_50Hz,50);	//�������
	wos_add(loop_1Hz,1);		//flash ��������
	eagle_startCapture();		//	��һ������
	wos_run();

}

