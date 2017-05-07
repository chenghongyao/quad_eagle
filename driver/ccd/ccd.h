#ifndef 	_CCD_H
#define		_CCD_H

#include "sys.h"




#define	CCD_PIXEL_TAR			150	
#define CCD_MAX_ERR				10		//最大允许偏差0.2*10V


typedef struct
{
	uint8_t image[128];	//图像数据
	uint8_t msCounter;	//ms计数器
	uint8_t intTime;		//曝光时间
	
	
	
	
	uint8_t lines[5];
	
	uint16_t	iLeft;		//左侧线
	uint16_t iRight;		//右侧线
	uint16_t iMid;			//中线
	
	
	
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

