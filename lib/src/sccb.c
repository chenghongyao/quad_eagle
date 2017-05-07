#include "sccb.h"
#include "sys.h"


void SCCB_Init()
{
	H_GPIO_Init(B,11, GPIO_Speed_50MHz,GPIO_Mode_Out_PP);//SCL
	H_GPIO_Init(B,10, GPIO_Speed_50MHz,GPIO_Mode_IPU);	//SDA			
	IIC_SDA_OUT();
}


static void SCCB_Delay( )
{
	delay_us(20);
}


static void SCCB_Start(void)
{	
	SCCB_SDA = 1;
	SCCB_SCL = 1;
	SCCB_Delay();
	SCCB_SDA = 0;	//下降沿
	SCCB_Delay();
	SCCB_SCL = 0;	
}


static void SCCB_Stop(void)
{
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
	SCCB_Delay();
	SCCB_SDA = 1;		
	SCCB_SCL = 1;			//SDA 高电平
	SCCB_Delay();
	SCCB_SCL = 0;
	SCCB_Delay();	
	SCCB_SDA = 0;
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
	SCCB_SCL = 1;	
	SCCB_Delay();
	if(SCCB_SDAI)res = 0;
	else res = 1;
	SCCB_SCL = 0;
	SCCB_SDA_OUT();
	return res;
}

static uint8_t SCCB_ReadByte()
{	
	uint8_t val,i;
	SCCB_SDA_IN();
	for(i = 0;i<8;i++)
	{	
		val<<=1;	
		SCCB_SCL = 1;
		SCCB_Delay();	
		if(SCCB_SDAI)val++;
		SCCB_SCL=0;
		SCCB_Delay();	
	}
	SCCB_SDA_OUT();
	return val;
}




uint8_t SCCB_WriteReg(uint8_t dev_addr,uint8_t reg_addr,uint8_t reg_val)
{
	SCCB_Start();
	if(0==SCCB_WriteByte(dev_addr&0xFE))return 0;
	if(0==SCCB_WriteByte(reg_addr))return 0;
	if(0==SCCB_WriteByte(reg_val))return 0;
	SCCB_Stop();
	return 1;
}

uint8_t SCCB_ReadReg(uint8_t dev_addr,uint8_t reg_addr,uint8_t *val)	//先读低字节
{
	SCCB_Start();
	if(0==SCCB_WriteByte(dev_addr&0xFE))return 0;
	if(0==SCCB_WriteByte(reg_addr))return 0;
	SCCB_Stop();
	
	SCCB_Start();
	if(0==SCCB_WriteByte(dev_addr|0x01))return 0;
	*val = SCCB_ReadByte();
	SCCB_NAck();
	SCCB_Stop();
	return 1;
}
