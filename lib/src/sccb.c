#include "sccb.h"
#include "sys.h"


void SCCB_Init()
{
	

	
	H_GPIO_Init(B,5, GPIO_Speed_50MHz,GPIO_Mode_Out_PP);//SCL
	H_GPIO_Init(B,6, GPIO_Speed_50MHz,GPIO_Mode_IPU);	//SDA			
	SCCB_SDA_OUT();
	
}


static void SCCB_Delay( )
{
	uint16_t time=200;
	while(time)
	{
		time--;
	}
//	delay_us(50);
}


static uint8_t SCCB_Start(void)
{	
	SCCB_SDA = 1;
	SCCB_SCL = 1;
	SCCB_Delay();
	SCCB_SDA_IN();
	if(!SCCB_SDAI)
	{
		SCCB_SDA_OUT();
		return 0;
	}
	SCCB_SDA_OUT();
	SCCB_SDA = 0;	//下降沿
	SCCB_Delay();
	SCCB_SCL = 0;	
	
	if(SCCB_SDAI)
	{
		SCCB_SDA_OUT();
		return 0;
	}
	return 1;
	
}


static void SCCB_Stop(void)
{
	SCCB_SCL = 0;
	SCCB_SDA = 0;
	SCCB_Delay();
	SCCB_SCL = 1;
	SCCB_Delay();
	SCCB_SDA = 1;
	SCCB_Delay();
	SCCB_SCL = 0;
}

static void SCCB_NAck(void)
{
	SCCB_SCL = 0;
	SCCB_Delay();
	SCCB_SDA = 1;		
	SCCB_Delay();
	SCCB_SCL = 1;			//SDA 高电平
	SCCB_Delay();
	SCCB_SCL = 0;
	SCCB_Delay();		
}


static uint8_t SCCB_WriteByte(uint8_t byte)
{
	uint8_t i,res = 1;
	for(i=0;i<8;i++)
	{
		if(byte&0x80)SCCB_SDA = 1;
		else 	SCCB_SDA = 0;
		SCCB_Delay();
		SCCB_SCL = 1;
		SCCB_Delay();
		SCCB_SCL = 0;
		SCCB_Delay();
		byte <<=1;
	}
	
	//检测应答信号
	SCCB_SDA_IN();
		SCCB_Delay();
	SCCB_SCL = 1;	
	SCCB_Delay();
	if(SCCB_SDAI)res = 0;//NACK
	else res = 1;					//ACK
	SCCB_SCL = 0;
	SCCB_SDA_OUT();
	return res;
}

static uint8_t SCCB_ReadByte()
{	
	uint8_t val=0,i;
	SCCB_SDA_IN();
	for(i = 0;i<8;i++)
	{	
		val<<=1;	
		SCCB_SCL = 0;
		SCCB_Delay();	
		SCCB_SCL = 1;
		SCCB_Delay();	
		if(SCCB_SDAI)val|=0x01;
	}
	SCCB_SDA_OUT();
	SCCB_SCL = 0;
	return val;
}




uint8_t SCCB_WriteReg(uint8_t dev_addr,uint8_t reg_addr,uint8_t reg_val)
{
	if(0 == SCCB_Start())return 0;
	if(0==SCCB_WriteByte(dev_addr&0xFE))
	{
		SCCB_Stop();
		return 0;
	};
	if(0==SCCB_WriteByte(reg_addr))return 0;
	if(0==SCCB_WriteByte(reg_val))return 0;
	SCCB_Stop();
	return 1;
}

uint8_t SCCB_ReadReg(uint8_t dev_addr,uint8_t reg_addr,uint8_t *val)	//先读低字节
{
	if(0 == SCCB_Start())return 0;
	if(0==SCCB_WriteByte(dev_addr&0xFE))
	{
		SCCB_Stop();
		return 0;
	};
	
	if(0==SCCB_WriteByte(reg_addr))return 0;
	SCCB_Stop();
	
	if(0 == SCCB_Start())return 0;
	if(0==SCCB_WriteByte(dev_addr|0x01))
	{
		SCCB_Stop();
		return 0;
	};
	
	*val = SCCB_ReadByte();
	SCCB_NAck();
	SCCB_Stop();
	return 1;
}
