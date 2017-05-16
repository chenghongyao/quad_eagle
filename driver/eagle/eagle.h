#ifndef _EAGLE_H_
#define _EAGLE_H_

#include "sys.h"



#define CAMERA_W 160		//宽度	
#define CAMERA_H 120		//高度
#define CAMERA_SIZE CAMERA_H*CAMERA_W/8		//高度


#define EAGLE_LEFT_MSB	0//1=高位在左,0=低位在左
#define UPLOAD_NRF	1			//1=使用NRF24L01,约100ms
													//0=使用串口,约200ms@115200,100ms@256000
typedef enum
{
	EAGLE_IDLE=0,
	EAGLE_START,
	EAGLE_GATHERING,
	EAGLE_FINISH,
	EAGLE_ERROR,
}eagle_status_t;


typedef struct
{
//	uint8_t *image;
	uint8_t *image_gather;
	uint8_t *image_use;
	uint32_t cnt;
	
	eagle_status_t status;
	uint8_t threshold;
}eagle_t;



extern volatile eagle_t meagle;


#define EAGLE_PUTC(_c)	H_USART_PUT8_F(1,_c)


uint8_t OV7725_WriteReg(uint8_t reg_addr,uint8_t reg_val);
uint8_t OV7725_ReadReg(uint8_t reg_addr,uint8_t *val);

uint8_t eagle_init(uint8_t *img_buffer1,uint8_t *img_buffer2);
uint8_t eagle_setThreshold(uint8_t val);
void eagle_startCapture(void);		//启动时会选用另一个缓冲区
void eagle_pauseCapture(void);


void eagle_prase(uint8_t *raw_buffer,uint8_t *prase_buffer);
void eagle_uploadImage(uint8_t *buffer);
uint8_t eagle_uploadImageAnsy(uint8_t *buffer);//发送完成返回1
uint8_t eagle_uploadRouteAnsy(uint8_t *buffer);

/****图像处理***/
void eagle_praseLine(uint8_t *img_buffer,uint8_t *line_buffer,uint16_t line_num);
#endif

