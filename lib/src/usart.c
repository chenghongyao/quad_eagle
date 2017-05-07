#include "usart.h"
#include "sys.h"
#include "ano.h"


void usart1_init()
{
	H_USART1_ENRCC();
	H_USART_Init(1,115200,USART_Mode_Rx | USART_Mode_Tx);
	H_USART_IT_RXNE(1,2,2);
	
	H_GPIO_Init(A,9,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
	H_GPIO_Init(A,10,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);

	
}

void usart2_init()
{
		/*USART2*/
	H_USARTx_ENRCC(2);
	H_USART_Init(2,115200,USART_Mode_Rx | USART_Mode_Tx);
	H_USART_IT_RXNE(2,2,2);
	
	H_GPIO_Init(A,2,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);			//TX
	H_GPIO_Init(A,3,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);	//RX

}


void usart3_init()
{
	H_USARTx_ENRCC(3);
	H_USART_Init(3,115200,USART_Mode_Rx | USART_Mode_Tx);
	H_USART_IT_RXNE(3,2,2);
	
	H_GPIO_Init(B,10,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);				//TX
	H_GPIO_Init(B,11,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);	//RX
}

void myputc(uint8_t c)
{
	H_USART_PUT8_F(1,c);
}


void myputbuf(uint8_t *buffer, uint16_t len)
{
	uint16_t i;
	for ( i = 0; i < len; i++)
	{
		myputc(*buffer++);
	}
}
void myputs(uint8_t *s)
{
	while(*s)
	{
		myputc(*s++);
	}
	myputc('\r');
	myputc('\n');
}





void usart3_putc(uint8_t c)
{
	H_USART_PUT8_F(3,c);
}



void usart3_putbuf(uint8_t *buffer, uint16_t len)
{
	uint16_t i;
	for ( i = 0; i < len; i++)
	{
		usart3_putc(*buffer++);
	}
}


//////////////////////////////////////////////////////////////////////////
void USART1_IRQHandler(void)
{

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t Res;
		Res = USART_ReceiveData(USART1);
		if(Res == 0x7f)
		{
//	
//			++isp_counter;
//			if(isp_counter == 5)
//			{
//				Sys_Soft_Reset();
//			}
//		}
//		else
//		{
//			isp_counter = 0;
		}
		//ANO_RecvByte(Res);
		//H_USART_PUT8_F(2,Res);
	}
}


void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		uint8_t Res;
		Res = USART_ReceiveData(USART2);//(USART1->DR); 
	//	queue_push(&qUart, Res);
	//	H_USART_PUT8_F(1, Res);
		if(Res)
		{
		
		}
	}
}



void USART3_IRQHandler(void) 
{
	if (USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		uint8_t Res;
		Res =USART3->DR; 
		//queue_push(&qUart, Res);
		//H_USART_PUT8_F(3, Res);
	}
}



