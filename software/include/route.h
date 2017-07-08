#ifndef _ROUTE_H_
#define _ROUTE_H_

#include "sys.h"
#include "eagle.h"



#define ROUTE_MIDPOS	(CAMERA_W/2)				//�е�λ��
typedef enum
{	
	ROUTE_NONE = 0,		//û������·��,ֱ��??
	ROUTE_LEFT,				//���
	ROUTE_RIGHT,			//�ҹ�
	ROUTE_CROSS,			//ʮ��
	ROUTE_CIRCLE,			//Բ��
}route_type_t;

uint16_t route_find(uint8_t *img_buffer,uint8_t *result,uint16_t line_start,uint16_t line_count,uint16_t search_start);
uint16_t route_find2(uint8_t *img_buffer,uint8_t *result,uint16_t line_start,uint16_t line_count,uint16_t search_start);
int16_t route_getAverageDistance(uint8_t *route,uint16_t line_start,uint16_t n);
float route_getSlope(uint8_t *route,uint16_t line_start,uint16_t n);

float calculation_variance(float a,float b, float c,float average);
float calculation_average(float a,float b, float c);
#endif

