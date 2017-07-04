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


/**************SCCB��д*******************************************************/
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
//		{OV7725_COM4         , 0xC1},
//		{OV7725_CLKRC        , 0x02},	//50֡
	
//		{OV7725_COM4         , 0x81},
//		{OV7725_CLKRC        , 0x00},//112֡
	
//		{OV7725_COM4         , 0xC1},
//		{OV7725_CLKRC        , 0x00},	//150֡
	
	
    {OV7725_COM4         , 0x41},	//37֡,1֡26ms
    {OV7725_CLKRC        , 0x01},
		
//		{OV7725_COM4         , 0x41},	//25֡
//    {OV7725_CLKRC        , 0x02},
		
    {OV7725_COM2         , 0x03},
    {OV7725_COM3         , 0xD0},//RGB��YUV��ʽ˳��,���λΪ��������
    {OV7725_COM7         , 0x40},//QVGA
    {OV7725_COM10        , 0x20},//
    {OV7725_HSTART       , 0x3F},//����ʼ
    {OV7725_HSIZE        , 0x50},//���ش�С
    {OV7725_VSTRT        , 0x03},//����ʼ����
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

    {OV7725_EXHCH        , 0x00},//�������ظ�λ����
    {OV7725_GAM1         , 0x0c},//gama��������
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
 //   {OV7725_CNST         , 0xFF}, //��ֵ
    {OV7725_SIGN         , 0x06},
    {OV7725_UVADJ0       , 0x11},
    {OV7725_UVADJ1       , 0x02},
};



volatile eagle_t meagle;//һ��Ҫ��volatile��ֹ�Ż�
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
	
	
	//��󲻳���1ms����ʱ
	//��ȡID
	if(0 == OV7725_ReadReg(OV7725_VER,&ver)) 
	{
		debug("��ȡidʧ��\r\n");
		return 0;
	}
	debug("VER=%2x\r\n",ver);
	
	
	
	if(ver == OV7725_VER_V)
	{
		for(i=0;i<reg_num;i++)
		{
			if(0 == OV7725_WriteReg(ov7725_eagle_reg[i].addr,ov7725_eagle_reg[i].val)) 
			{
				debug("�Ĵ���д��ʧ��\r\n");
				return 0;
			}
			
			if(0 == eagle_setThreshold(meagle.threshold))
			{
				return 0;
			}
		}
		
	}
	else
	{
		return 0;
	}
	return 1;
	
}

void eagle_initDma(uint8_t *img_buf,uint32_t len)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&GPIOA->IDR);					//�����ַ PA��
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		  //8λ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//�����ַ����
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)img_buf;							//�ڴ��ַ
	DMA_InitStructure.DMA_BufferSize = len;											//�ڴ��С
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;							//���ݿ��
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//�ڴ��ַ����ʹ��
	
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;													//�������赽�ڴ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 															//��������
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;											//�����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//�ڴ浽�ڴ�	
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);																	//TIM3_CH4��ͨ��3
}


uint8_t eagle_init(uint8_t *img_buffer1,uint8_t *img_buffer2)
{
	meagle.image_gather = img_buffer1;
	meagle.image_use = img_buffer2;
	meagle.status = EAGLE_IDLE;
	meagle.threshold = mTab.threshold;
	if(0 ==	eagle_initReg())
	{
		debug("OV7725�Ĵ�����ʼ��ʧ��\r\n");
		return 0;
	}
	debug("OV7725�Ĵ�����ʼ���ɹ�\r\n");
	return 1;
}



//���÷�ֵ
uint8_t eagle_setThreshold(uint8_t val)
{
		if(0 == OV7725_WriteReg(OV7725_CNST,val)) 
		{
			debug("�Ĵ���д��ʧ��\r\n");
			return 0;
		}
		
		return 1;
}

void eagle_startCapture()
{
	uint8_t *p = meagle.image_gather;
	meagle.image_gather = meagle.image_use;
	meagle.image_use = p;
	
	meagle.status = EAGLE_START;												//������־λ
	DMA_Cmd(DMA1_Channel3,DISABLE);											//�ر�DMA��������
	eagle_initDma(meagle.image_gather,CAMERA_SIZE);
	

	EXTI_ClearFlag(EXTI_Line10);
	H_EXTI_IT_ENABLE(10);	//�����ж�
}


void eagle_pauseCapture()
{
	TIM_DMACmd(TIM3,TIM_DMA_CC4,DISABLE);								//�رն�ʱ������
	H_EXTI_IT_DISABLE(10);															//�س��ж�
}





/***********************ͼ����********************/

void eagle_uploadImage(uint8_t *buffer)
{
	#define IMG_CMD	1
	uint16_t i;
	uint8_t cmd1[2]={IMG_CMD,~IMG_CMD};
	uint8_t cmd2[2]={~IMG_CMD,IMG_CMD};
	uint8_t *p= buffer;
#if	(UPLOAD_NRF == 1)
	nrf24l01_sendPacket(cmd1,2);
	
	for(i=0;i<75;i++)
	{
		nrf24l01_sendPacket(p,32);
		p+=32;
	}
	nrf24l01_sendPacket(cmd2,2);
#else
	EAGLE_PUTC(cmd1[0]);
	EAGLE_PUTC(cmd1[1]);
	for(i=0;i<CAMERA_SIZE;i++)
	{
		EAGLE_PUTC(*p);
		p++;
	}
	EAGLE_PUTC(cmd2[0]);
	EAGLE_PUTC(cmd2[1]);
#endif
}


//������ɷ���1
uint8_t eagle_uploadImageAnsy(uint8_t *buffer)
{
	static uint8_t step = 0;
	static uint8_t *p;
	static uint16_t cnt;
	uint8_t len;
	
	uint8_t cmd1[2]={0x01,0xFE};
	uint8_t cmd2[2]={0xFE,0x01};

	if(step==0)
	{
		if(0==nrf24l01_sendPacket(cmd1,2))return 0;
		p = buffer;
		cnt = CAMERA_SIZE;
		step ++;
	}
	else if(step == 1)
	{
		len = (cnt >32)?32:cnt;//ʣ������32ȡ��Сֵ
		if(0==nrf24l01_sendPacket(p,len))return 0;
		p+=len;
		cnt-=len;
		if(cnt==0)	//�������
		{
			step ++;
		}
	}
	else
	{
		if(0==nrf24l01_sendPacket(cmd2,2))return 0;
		step = 0;
		return 1;				//�������
	}
	
	return 0;
}


//������ɷ���1
uint8_t eagle_uploadRouteAnsy(uint8_t *buffer)
{
	static uint8_t step = 0;
	static uint8_t *p;
	static uint16_t cnt;
	uint8_t len;
	uint8_t cmd1[2]={0x02,0xFD};
	uint8_t cmd2[2]={0xFD,0x02};

	if(step==0)
	{
		nrf24l01_sendPacket(cmd1,2);
		p = buffer;
		cnt = CAMERA_H;
		step ++;
	}
	else if(step == 1)
	{
		
		len = (cnt >32)?32:cnt;//ʣ������32ȡ��Сֵ
		nrf24l01_sendPacket(p,len);

		p+=len;
		cnt-=len;
		if(cnt==0)	//�������
		{
			step ++;
		}
	}
	else
	{
		nrf24l01_sendPacket(cmd2,2);
		step = 0;
		return 1;
	}
	return 0;
}


//��ѹ����ͼ��
void eagle_prase(uint8_t *raw_buffer,uint8_t *prase_buffer)
{
	uint16_t h,w;
	uint8_t *praw = raw_buffer;
	uint8_t *pprase = prase_buffer;
	//��λ����
	for(h=0;h<CAMERA_H;h++)
	{
		for(w=0;w<(CAMERA_W/8);w++)
		{
#if EAGLE_LEFT_MSB == 1		//��λ����
			*pprase++= ((*praw&0x80)==0)?0:0xff;
			*pprase++= ((*praw&0x40)==0)?0:0xff;
			*pprase++= ((*praw&0x20)==0)?0:0xff;
			*pprase++= ((*praw&0x10)==0)?0:0xff;
			*pprase++= ((*praw&0x08)==0)?0:0xff;
			*pprase++= ((*praw&0x04)==0)?0:0xff;
			*pprase++= ((*praw&0x02)==0)?0:0xff;
			*pprase++= ((*praw&0x01)==0)?0:0xff;		
#else//��λ����	
		*pprase++= ((*praw&0x01)==0)?0:0xff;
		*pprase++= ((*praw&0x02)==0)?0:0xff;
		*pprase++= ((*praw&0x04)==0)?0:0xff;
		*pprase++= ((*praw&0x08)==0)?0:0xff;
		*pprase++= ((*praw&0x10)==0)?0:0xff;
		*pprase++= ((*praw&0x20)==0)?0:0xff;
		*pprase++= ((*praw&0x40)==0)?0:0xff;
		*pprase++= ((*praw&0x80)==0)?0:0xff;
#endif
		}
	}
}


//img_buffer:ԭʼͼ������(δ��ѹ)
void eagle_praseLine(uint8_t *img_buffer,uint8_t *line_buffer,uint16_t line_num)
{
	uint16_t w;
	uint8_t *praw;
	praw = img_buffer + line_num*(CAMERA_W/8);		//ָ�������	

	for(w=0;w<(CAMERA_W/8);w++)
	{
#if EAGLE_LEFT_MSB == 1		//��λ����
		*line_buffer++= ((*praw&0x80)==0)?0:0xff;
		*line_buffer++= ((*praw&0x40)==0)?0:0xff;
		*line_buffer++= ((*praw&0x20)==0)?0:0xff;
		*line_buffer++= ((*praw&0x10)==0)?0:0xff;
		*line_buffer++= ((*praw&0x08)==0)?0:0xff;
		*line_buffer++= ((*praw&0x04)==0)?0:0xff;
		*line_buffer++= ((*praw&0x02)==0)?0:0xff;
		*line_buffer++= ((*praw&0x01)==0)?0:0xff;		
#else//��λ����	
		*line_buffer++= (((*praw)&0x01)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x02)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x04)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x08)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x10)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x20)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x40)==0)?0:0xff;
		*line_buffer++= (((*praw)&0x80)==0)?0:0xff;
#endif
		praw++;
	}
}


