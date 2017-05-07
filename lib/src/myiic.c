#include "myiic.h"
#include "sys.h"



void IIC_delay(void)			//����ͨ�������ɽ����ٶ����Ĵ˺���
{
    uint8_t  i = 7;//��������Ż��ٶ�	����������͵�5����д��
    while (i)i--;
}



void IIC_Init(void)
{
	//�ȳ�ʼ��SCL�ڳ�ʼ��SDAû����,�ȳ�ʼ��SDA,SCL������!!!FUCK
	H_GPIO_Init(A,5, GPIO_Speed_50MHz,GPIO_Mode_Out_PP);//SCL
	H_GPIO_Init(A,4, GPIO_Speed_50MHz,GPIO_Mode_Out_PP);//SDA																	//SDA
}






void IIC_Start(void)
{	
	IIC_SDA = 1;
	IIC_SCL = 1;
	IIC_delay();
	IIC_SDA = 0;	//�½���
	IIC_delay();
	IIC_SCL = 0;	
}



void IIC_Stop(void)
{
	IIC_SDA = 0;
	IIC_SCL = 1;
	IIC_delay();
	IIC_SDA = 1;
	IIC_delay();
	IIC_SCL = 0;
}

uint8_t IIC_WaitAck(void)
{
	uint8_t t=0;
	IIC_SDA_IN();
	IIC_SCL = 1;	
	IIC_delay();
	while(IIC_SDAI)
	{
		if(++t>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_delay();
	IIC_SCL = 0;
	IIC_SDA_OUT();
	return 0;

}

void IIC_Ack(void)
{
	IIC_SDA = 0;
	IIC_SCL = 1;		//SDA ���ֵ͵�ƽ
	IIC_delay();
	IIC_SCL = 0;	
}

void IIC_NAck(void)
{
	IIC_SDA = 1;		
	IIC_SCL = 1;			//SDA �ߵ�ƽ
	IIC_delay();
	IIC_SCL = 0;		
}


void IIC_WriteByte(uint8_t byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		IIC_SDA = (byte&0x80)>>7;
		IIC_SCL = 1;
		IIC_delay();
		IIC_SCL = 0;
		IIC_delay();	
		byte <<=1;
	}
}


uint8_t IIC_ReadByte(uint8_t ack)
{	
	uint8_t val,i;
	IIC_SDA_IN();
	for(i = 0;i<8;i++)
	{	
		val<<=1;	
		IIC_SCL = 1;
		IIC_delay();	
		if(IIC_SDAI)val++;
		IIC_SCL=0;
		IIC_delay();	
	}
	IIC_SDA_OUT();
	if(ack)	IIC_Ack();
	else		IIC_NAck();
	
	return val;
}

//=================================================================================================
uint8_t IIC_DevWriteByte(uint8_t dev_addr,uint8_t reg_addr,uint8_t reg_val)
{
	IIC_Start();
	IIC_WriteByte(dev_addr& 0xfe);
	if(IIC_WaitAck())
	{
		return 1;
	}
	IIC_WriteByte(reg_addr);
	if(IIC_WaitAck())return 1;
	IIC_WriteByte(reg_val);
	if(IIC_WaitAck())return 1;
	IIC_Stop();
	
	return 0;
}
uint8_t IIC_DevReadByte(uint8_t dev_addr,uint8_t reg_addr)
{
	uint8_t reg_val;
	
	IIC_Start();
	IIC_WriteByte(dev_addr & 0xFE);
	if(IIC_WaitAck())return 1;
	IIC_WriteByte(reg_addr);
	if(IIC_WaitAck())return 1;
	
	IIC_Start();
	IIC_WriteByte(dev_addr|0x01);
	if(IIC_WaitAck())return 1;
	reg_val = IIC_ReadByte(0);	//������Ӧ��
	IIC_Stop();
	return reg_val;
}

uint8_t IIC_DevReadBuf(uint8_t dev_addr,uint8_t reg_addr,uint8_t len,uint8_t* pBuf)	//�ȶ����ֽ�
{
	uint8_t i;
	IIC_Start();
	IIC_WriteByte(dev_addr & 0xFE);
	if(IIC_WaitAck())return 1;
	IIC_WriteByte(reg_addr);
	if(IIC_WaitAck())return 1;
	
	IIC_Start();
	IIC_WriteByte(dev_addr|0x01);
	if(IIC_WaitAck())return 1;
	len--;
	for(i=0;i<len;i++)
	{
		pBuf[i] = IIC_ReadByte(1);	//����Ӧ��
	}
	pBuf[i] = IIC_ReadByte(0);	//������Ӧ��
	IIC_Stop();
	return 0;
}
//==========================================================================================






















