#ifndef _MS5611_H
#define _MS5611_H
#include "sys.h"




#define MS5611Press_OSR  MS561101BA_OSR_4096  //��ѹ��������
#define MS5611Temp_OSR   MS561101BA_OSR_4096  //�¶Ȳ�������
#define MS5611_PREINIT_NUM 	300
#define MS5611_INIT_NUM 		100


typedef struct
{
	uint8_t IsBaroUpdated;
	
	float Temperature;			//�¶� ��
	float Pressure;					//ѹ�� ��
	float PressureZero;			//0m��ѹֵ
	float Altitude;					//�߶� m
	float alt_raw;
	float alt_offset;
	uint8_t PressInited;
}ms5611_t;
extern ms5611_t mMS5611;

void ms5611_init(void);
void ms5611_updateThread(void);
float ms5611_getAltitude(void);
//uint8_t ms5611_waitBroInitOffset(void);






#endif

