#include "sys.h"
#include "usart.h"

#include "wos.h"
#include "rc.h"
#include "mat.h"
#include "imu.h"
#include "ano.h"
#include "flight.h"
#include "controller.h"

#include "alt.h"
#include "ccd.h"
#include "csp.h"


void loop_10Hz()
{
	ledM = !ledM;
}


volatile uint8_t eagle_buffer[CAMERA_SIZE];
void bufferinit()
{
	uint16_t i,j;
	volatile uint8_t *p = eagle_buffer;

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


void uploadImage()
{
	uint16_t i;
	uint8_t cmd = 1;
	uint16_t len = CAMERA_SIZE;
	volatile uint8_t *buf = eagle_buffer;
	EAGLE_PUTC(cmd);
	EAGLE_PUTC(~cmd);
	for(i=0;i<len;i++)
	{
		EAGLE_PUTC(*buf);
		buf++;
	}
	EAGLE_PUTC(~cmd);
	EAGLE_PUTC(cmd);
}
int main(void)
{
	uint16_t i,res = 1;
	board_setup();
	debug("³õÊ¼»¯...\r\n");
	delay_ms(1000);
	eagle_init();
	bufferinit();
	uploadImage();
	delay_ms(1000);
	
	while(1)
	{
		eagle_startCapture();
		while(meagle.hasUpdate == 0);
		meagle.hasUpdate = 0;
		uploadImage();
	}
}


