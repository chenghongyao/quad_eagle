#ifndef _EAGLE_H_
#define _EAGLE_H_

#include "sys.h"



#define CAMERA_W 160		//宽度	
#define CAMERA_H 120		//高度
#define CAMERA_SIZE CAMERA_H*CAMERA_W/8		//高度


typedef struct
{
	volatile uint8_t *image;
	uint32_t cnt;
	uint8_t hasUpdate;
	uint8_t fStart;
	
	uint32_t t1;
	uint32_t t2;
	uint16_t cntline;
}eagle_t;

extern volatile eagle_t meagle;




#define EAGLE_PUTC(_c)	H_USART_PUT8_F(1,_c)





uint8_t eagle_init(void);
void eagle_uploadImage(void);
void eagle_startCapture(void);
void eagle_pauseCapture(void);

#endif

