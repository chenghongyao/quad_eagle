#include "eagle.h"
#include "sys.h"

#include "ov7725_dev.h"
#include "sccb.h"
/********************

1.SCCB
�豸��ַ0x42
�������400k
2.��ÿһ���Ĵ����ĸı�,��Ҫ���300ms���ӳ�
3.ΨһID
������ID PID=77H,VER=21H
������ID MIDH=7FH,MIDL=A2H
4.
********************/


/*********************************************************************/
uint8_t OV7725_WriteReg(uint8_t reg_addr,uint8_t reg_val)
{
	uint8_t i;
	for(i=0;i<10;i++)
	{
		if(SCCB_WriteReg(OV7725_ADDR,reg_addr,reg_val))
		{
			return 1;
		}
	}
	return 0;
	
}

uint8_t OV7725_ReadReg(uint8_t reg_addr,uint8_t *val)
{
	uint8_t i;
	for(i=0;i<10;i++)
	{
		if(SCCB_ReadReg(OV7725_ADDR,reg_addr,val))
		{
			return 1;
		}
	}
	return 0;
}
/************************************************************************/



/*OV7725��ʼ�����ñ�*/
ov7725_reg_t ov7725_eagle_reg[] =
{
    //�Ĵ���,�Ĵ���ֵ
    {OV7725_COM4         , 0x41},
    {OV7725_CLKRC        , 0x08},
		
    {OV7725_COM2         , 0x03},
    {OV7725_COM3         , 0xD0},
    {OV7725_COM7         , 0x40},
    {OV7725_COM10        , 0x20},
    {OV7725_HSTART       , 0x3F},
    {OV7725_HSIZE        , 0x50},
    {OV7725_VSTRT        , 0x03},
    {OV7725_VSIZE        , 0x78},
    {OV7725_HREF         , 0x00},
    {OV7725_SCAL0        , 0x0A},
    {OV7725_AWB_Ctrl0    , 0xE0},
    {OV7725_DSPAuto      , 0xff},
    {OV7725_DSP_Ctrl2    , 0x0C},
    {OV7725_DSP_Ctrl3    , 0x00},
    {OV7725_DSP_Ctrl4    , 0x00},
		

#if (CAMERA_W == 80)
    {OV7725_HOutSize     , 0x14},
#elif (CAMERA_W == 160)
    {OV7725_HOutSize     , 0x28},
#elif (CAMERA_W == 240)
    {OV7725_HOutSize     , 0x3c},
#elif (CAMERA_W == 320)
    {OV7725_HOutSize     , 0x50},
#else

#endif

#if (CAMERA_H == 60 )
    {OV7725_VOutSize     , 0x1E},
#elif (CAMERA_H == 120 )
    {OV7725_VOutSize     , 0x3c},
#elif (CAMERA_H == 180 )
    {OV7725_VOutSize     , 0x5a},
#elif (CAMERA_H == 240 )
    {OV7725_VOutSize     , 0x78},
#else

#endif

    {OV7725_EXHCH        , 0x00},
    {OV7725_GAM1         , 0x0c},
    {OV7725_GAM2         , 0x16},
    {OV7725_GAM3         , 0x2a},
    {OV7725_GAM4         , 0x4e},
    {OV7725_GAM5         , 0x61},
    {OV7725_GAM6         , 0x6f},
    {OV7725_GAM7         , 0x7b},
    {OV7725_GAM8         , 0x86},
    {OV7725_GAM9         , 0x8e},
    {OV7725_GAM10        , 0x97},
    {OV7725_GAM11        , 0xa4},
    {OV7725_GAM12        , 0xaf},
    {OV7725_GAM13        , 0xc5},
    {OV7725_GAM14        , 0xd7},
    {OV7725_GAM15        , 0xe8},
    {OV7725_SLOP         , 0x20},
    {OV7725_LC_RADI      , 0x00},
    {OV7725_LC_COEF      , 0x13},
    {OV7725_LC_XC        , 0x08},
    {OV7725_LC_COEFB     , 0x14},
    {OV7725_LC_COEFR     , 0x17},
    {OV7725_LC_CTR       , 0x05},
    {OV7725_BDBase       , 0x99},
    {OV7725_BDMStep      , 0x03},
    {OV7725_SDE          , 0x04},
    {OV7725_BRIGHT       , 0x00},
    {OV7725_CNST         , 0xFF},
    {OV7725_SIGN         , 0x06},
    {OV7725_UVADJ0       , 0x11},
    {OV7725_UVADJ1       , 0x02},
};



volatile eagle_t meagle;//һ��Ҫ��volatile
uint8_t eagle_initReg(void)
{
	uint16_t reg_num = sizeof(ov7725_eagle_reg)/sizeof(ov7725_reg_t);
	uint16_t i;
	uint8_t ver;
	//��λ
	if(0 == OV7725_WriteReg(OV7725_COM7,0x80)) 
	{
		debug("��λʧ��\r\n");
		return 0;
	}
	debug("��λ�ɹ�\r\n");
	delay_ms(100);
	
	//��ȡID
	if(0 == OV7725_ReadReg(OV7725_VER,&ver)) 
	{
		debug("��ȡidʧ��\r\n");
		return 0;
	}
	debug("VER=%2x\r\n",ver);
	
	if(ver == OV7725_VER_V)
	{
		printf("i=%d,size=%d\r\n",reg_num,sizeof(ov7725_eagle_reg)/sizeof(ov7725_reg_t));
		for(i=0;i<reg_num;i++)
		{
//			if(0 == OV7725_ReadReg(ov7725_eagle_reg[i].addr,&ver)) 
//			{
//				debug("�Ĵ���%2x��ȡʧ��\r\n",ov7725_eagle_reg[i].addr);
//				return 0;
//			}
//			debug("�Ĵ���%2x=%x\r\n",ov7725_eagle_reg[i].addr,ver);
			
			if(0 == OV7725_WriteReg(ov7725_eagle_reg[i].addr,ov7725_eagle_reg[i].val)) 
			{
				debug("�Ĵ���д��ʧ��\r\n");
				return 0;
			}
			
			delay_ms(100);
//			if(0 == OV7725_ReadReg(ov7725_eagle_reg[i].addr,&ver)) 
//			{
//				debug("�Ĵ���%2x��ȡʧ��\r\n",ov7725_eagle_reg[i].addr);
//				return 0;
//			}
//			
//			if(ov7725_eagle_reg[i].val != ver)
//			{
//					debug("�Ĵ���%x���ʧ��-%x\r\n",ov7725_eagle_reg[i].addr,ver);
//			//	return 0;
//			}
		}
	}
	else
	{
		return 0;
	}
	return 1;
	
}
uint8_t eagle_init(uint8_t *img_buffer)
{
	meagle.image = img_buffer;
	meagle.hasUpdate = 0;
	if(0 ==	eagle_initReg())
	{
		debug("OV7725�Ĵ�����ʼ��ʧ��\r\n");
		return 0;
	}
	debug("OV7725�Ĵ�����ʼ���ɹ�\r\n");
	return 1;
}


void eagle_upload(uint8_t cmd,uint8_t *buf,uint16_t len)
{
	uint16_t i;
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

void eagle_uploadImage()
{
	uint16_t i;
	uint8_t cmd = 1;
	uint16_t len = CAMERA_SIZE;
	uint8_t *buf = meagle.image;
	EAGLE_PUTC(cmd);
	EAGLE_PUTC(~cmd);
	buf++;buf++;
	for(i=0;i<len;i++)
	{
		EAGLE_PUTC(*buf);
		buf++;
	}
	EAGLE_PUTC(~cmd);
	EAGLE_PUTC(cmd);
}



void eagle_startCapture()
{
	meagle.fStart = 1;
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel3,CAMERA_SIZE);
	EXTI_ClearFlag(EXTI_Line15);
	H_EXTI_IT_ENABLE(15_10,0,0);	//�����ж�
}





void eagle_pauseCapture()
{
	H_EXTI_IT_DISABLE(15_10);					//�س��ж�
}


