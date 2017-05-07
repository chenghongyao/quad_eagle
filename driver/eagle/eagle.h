#ifndef _EAGLE_H_
#define _EAGLE_H_

#include "sys.h"



#define CAMERA_W 160		//宽度	
#define CAMERA_H 120		//高度
#define CAMERA_SIZE CAMERA_H*CAMERA_W/8		//高度


typedef struct
{
	uint8_t *image;
	uint32_t cnt;
	uint8_t hasUpdate;
	uint8_t fStart;
}eagle_t;

extern volatile eagle_t meagle;




#define EAGLE_PUTC(_c)	H_USART_PUT8_F(1,_c)


uint8_t OV7725_WriteReg(uint8_t reg_addr,uint8_t reg_val);
uint8_t OV7725_ReadReg(uint8_t reg_addr,uint8_t *val);

uint8_t eagle_init(uint8_t *img_buffer);
void eagle_uploadImage(void);
void eagle_startCapture(void);
void eagle_pauseCapture(void);

#endif

