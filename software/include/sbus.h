#ifndef _SBUS_H_
#define _SBUS_H_

#include "sys.h"

#pragma pack(1) 
//位域,先定义在高位
typedef union
{
	uint8_t buffer[25];//startbyte,data1,data2,......,data22,flags,endbyte
	struct
	{
		uint8_t startbyte;//0x0f
		uint32_t ch1:11;
		uint32_t ch2:11;
		uint32_t ch3:11;
		uint32_t ch4:11;
		uint32_t ch5:11;
		uint32_t ch6:11;
		uint32_t ch7:11;
		uint32_t ch8:11;
		uint32_t ch9:11;
		uint32_t ch10:11;
		uint32_t ch11:11;
		uint32_t ch12:11;
		uint32_t ch13:11;
		uint32_t ch14:11;
		uint32_t ch15:11;
		uint32_t ch16:11;	
		
		uint8_t dg1:1;				//digital channel (0x80)
		uint8_t dg2:1;				//digital channel (0x40)
		uint8_t framelost:1;	// Frame lost, equivalent red LED on receiver (0x20)
		uint8_t failsafe:1;		//failsafe activated (0x10)
		uint8_t reserve:4;
		uint8_t endbyte;			//0x00
	}channel;
}sbus_t;
#pragma pack() 

/*
		{
			USART_InitTypeDef USART_InitStruct = {100000,USART_WordLength_9b,USART_StopBits_2,USART_Parity_Even,USART_Mode_Rx | USART_Mode_Tx,USART_HardwareFlowControl_None};																	
			USART_Init(USART1,&USART_InitStruct);
			USART_Cmd(USART1,ENABLE);
		}
		tNow = micros();	
		if(rxfinish == 0)
		{
			if((tNow - tPre)>3000)
			{
				if((rxcnt==25) && (msbus.buffer[0] == 0x0f) && (msbus.buffer[24]==0x00))
				{
					rxfinish = 1;
				}				
				rxcnt = 0;		
			}
			if(rxcnt<25)
			{
				msbus.buffer[rxcnt] = Res;
			}
			rxcnt++;
		}
		tPre = tNow;
		
		
		
	if(rxfinish == 1)
	{
		printf("1=%d,2=%d,3=%d,4=%d,\r\n",msbus.channel.ch1,msbus.channel.ch2,msbus.channel.ch3,msbus.channel.ch4);
		printf("5=%d,6=%d,7=%d,8=%d,\r\n",msbus.channel.ch5,msbus.channel.ch6,msbus.channel.ch7,msbus.channel.ch8);
		printf("9=%d,10=%d,11=%d,12=%d,\r\n",msbus.channel.ch9,msbus.channel.ch10,msbus.channel.ch11,msbus.channel.ch12);
		printf("13=%d,14=%d,15=%d,16=%d,\r\n",msbus.channel.ch13,msbus.channel.ch14,msbus.channel.ch15,msbus.channel.ch16);
		printf("dg1=%d,dg2=%d,lost=%d\r\n",msbus.channel.dg1,msbus.channel.dg2,msbus.channel.framelost);
		rxfinish = 0;
	}

*/


void sbus_init(void);

#endif

