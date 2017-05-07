#ifndef 	_CCD_H
#define		_CCD_H

#include "sys.h"




#define	CCD_PIXEL_TAR			150	
#define CCD_MAX_ERR				10		//�������ƫ��0.2*10V


typedef struct
{
	uint8_t image[128];	//ͼ������
	uint8_t msCounter;	//ms������
	uint8_t intTime;		//�ع�ʱ��
	
	
	
	
	uint8_t lines[5];
	
	uint16_t	iLeft;		//�����
	uint16_t iRight;		//�Ҳ���
	uint16_t iMid;			//����
	
	
	
}ccd_t;

extern ccd_t mccd;
void ccd_init(void);
uint8_t ccd_calcImageAverage(void);
void ccd_updateIntegrationTime(void);
void ccd_startIntegration(void);
void ccd_readImage(void);
void ccd_update(void);
void ccd_fineLine(void);
#endif

