#ifndef _SBUS_H_
#define _SBUS_H_


#include "sys.h"


typedef struct
{
	uint8_t reserve:4;
	uint8_t failsafe:1;
	uint8_t framelost:1;
	uint8_t ch18:1;
	uint8_t ch17:1;
}sbus_flags_t;
typedef union
{
	uint8_t buffer[25];//startbyte,data1,data2,......,data22,flags,endbyte
	struct
	{
		uint8_t startbyte;
		uint16_t ch1:11;
		uint16_t ch2:11;
		uint16_t ch3:11;
		uint16_t ch4:11;
		uint16_t ch5:11;
		uint16_t ch6:11;
		uint16_t ch7:11;
		uint16_t ch8:11;
		uint16_t ch9:11;
		uint16_t ch10:11;
		uint16_t ch11:11;
		uint16_t ch12:11;
		uint16_t ch13:11;
		uint16_t ch14:11;
		uint16_t ch15:11;
		uint16_t ch16:11;	
		sbus_flags_t flags;
		uint8_t endbyte;
	}channel;
}sbus_t;

void sbus_init(void);

#endif

