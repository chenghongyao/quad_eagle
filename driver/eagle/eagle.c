#include "eagle.h"
#include "sys.h"

#include "ov7725_dev.h"
#include "sccb.h"
/********************

1.SCCB
设备地址0x42
最大速率400k
2.对每一个寄存器的改变,需要最大300ms的延迟
3.唯一ID
传感器ID PID=77H,VER=21H
制造商ID MIDH=7FH,MIDL=A2H
4.
********************/


/**************SCCB读写*******************************************************/
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



/*OV7725初始化配置表*/
ov7725_reg_t ov7725_eagle_reg[] =
{
    //寄存器,寄存器值
//		{OV7725_COM4         , 0xC1},
//		{OV7725_CLKRC        , 0x02},	//50帧
	
//		{OV7725_COM4         , 0x81},
//		{OV7725_CLKRC        , 0x00},//112帧
	
//		{OV7725_COM4         , 0xC1},
//		{OV7725_CLKRC        , 0x00},	//150帧
	
	
    {OV7725_COM4         , 0x41},	//37帧,1帧26ms
    {OV7725_CLKRC        , 0x01},
		
//		{OV7725_COM4         , 0x41},	//25帧
//    {OV7725_CLKRC        , 0x02},
		
    {OV7725_COM2         , 0x03},
    {OV7725_COM3         , 0xD0},//RGB和YUV格式顺序,最低位为彩条测试
    {OV7725_COM7         , 0x40},//QVGA
    {OV7725_COM10        , 0x20},//
    {OV7725_HSTART       , 0x3F},//行起始
    {OV7725_HSIZE        , 0x50},//像素大小
    {OV7725_VSTRT        , 0x03},//场起始控制
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

    {OV7725_EXHCH        , 0x00},//虚拟像素高位插入
    {OV7725_GAM1         , 0x0c},//gama参数设置
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
 //   {OV7725_CNST         , 0xFF}, //阀值
    {OV7725_SIGN         , 0x06},
    {OV7725_UVADJ0       , 0x11},
    {OV7725_UVADJ1       , 0x02},
};



volatile eagle_t meagle;//一定要加volatile防止优化
uint8_t eagle_initReg(void)
{
	uint16_t reg_num = sizeof(ov7725_eagle_reg)/sizeof(ov7725_reg_t);
	uint16_t i;
	uint8_t ver;
	
	//复位
	if(0 == OV7725_WriteReg(OV7725_COM7,0x80)) 
	{
		debug("复位失败\r\n");
		return 0;
	}
	debug("复位成功\r\n");
	
	
	//最大不超过1ms的延时
	//读取ID
	if(0 == OV7725_ReadReg(OV7725_VER,&ver)) 
	{
		debug("读取id失败\r\n");
		return 0;
	}
	debug("VER=%2x\r\n",ver);
	
	
	
	if(ver == OV7725_VER_V)
	{
		for(i=0;i<reg_num;i++)
		{
			if(0 == OV7725_WriteReg(ov7725_eagle_reg[i].addr,ov7725_eagle_reg[i].val)) 
			{
				debug("寄存器写入失败\r\n");
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

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&GPIOA->IDR);					//外设地址 PA口
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		  //8位
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设地址不变
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)img_buf;							//内存地址
	DMA_InitStructure.DMA_BufferSize = len;											//内存大小
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;							//数据宽度
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//内存地址自增使能
	
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;													//单向，外设到内存
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 															//连续接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;											//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//内存到内存	
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);																	//TIM3_CH4在通道3
}


uint8_t eagle_init(uint8_t *img_buffer1,uint8_t *img_buffer2)
{
	meagle.image_gather = img_buffer1;
	meagle.image_use = img_buffer2;
	meagle.status = EAGLE_IDLE;
	meagle.threshold = mTab.threshold;
	if(0 ==	eagle_initReg())
	{
		debug("OV7725寄存器初始化失败\r\n");
		return 0;
	}
	debug("OV7725寄存器初始化成功\r\n");
	return 1;
}



//设置阀值
uint8_t eagle_setThreshold(uint8_t val)
{
		if(0 == OV7725_WriteReg(OV7725_CNST,val)) 
		{
			debug("寄存器写入失败\r\n");
			return 0;
		}
		
		return 1;
}

void eagle_startCapture()
{
	uint8_t *p = meagle.image_gather;
	meagle.image_gather = meagle.image_use;
	meagle.image_use = p;
	
	meagle.status = EAGLE_START;												//启动标志位
	DMA_Cmd(DMA1_Channel3,DISABLE);											//关闭DMA重新设置
	eagle_initDma(meagle.image_gather,CAMERA_SIZE);
	

	EXTI_ClearFlag(EXTI_Line10);
	H_EXTI_IT_ENABLE(10);	//开场中断
}


void eagle_pauseCapture()
{
	TIM_DMACmd(TIM3,TIM_DMA_CC4,DISABLE);								//关闭定时器触发
	H_EXTI_IT_DISABLE(10);															//关场中断
}





/***********************图像处理********************/

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


//发送完成返回1
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
		len = (cnt >32)?32:cnt;//剩余量和32取最小值
		if(0==nrf24l01_sendPacket(p,len))return 0;
		p+=len;
		cnt-=len;
		if(cnt==0)	//发送完成
		{
			step ++;
		}
	}
	else
	{
		if(0==nrf24l01_sendPacket(cmd2,2))return 0;
		step = 0;
		return 1;				//发送完成
	}
	
	return 0;
}


//发送完成返回1
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
		
		len = (cnt >32)?32:cnt;//剩余量和32取最小值
		nrf24l01_sendPacket(p,len);

		p+=len;
		cnt-=len;
		if(cnt==0)	//发送完成
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


//解压整副图像
void eagle_prase(uint8_t *raw_buffer,uint8_t *prase_buffer)
{
	uint16_t h,w;
	uint8_t *praw = raw_buffer;
	uint8_t *pprase = prase_buffer;
	//低位在左
	for(h=0;h<CAMERA_H;h++)
	{
		for(w=0;w<(CAMERA_W/8);w++)
		{
#if EAGLE_LEFT_MSB == 1		//高位在左
			*pprase++= ((*praw&0x80)==0)?0:0xff;
			*pprase++= ((*praw&0x40)==0)?0:0xff;
			*pprase++= ((*praw&0x20)==0)?0:0xff;
			*pprase++= ((*praw&0x10)==0)?0:0xff;
			*pprase++= ((*praw&0x08)==0)?0:0xff;
			*pprase++= ((*praw&0x04)==0)?0:0xff;
			*pprase++= ((*praw&0x02)==0)?0:0xff;
			*pprase++= ((*praw&0x01)==0)?0:0xff;		
#else//低位在左	
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


//img_buffer:原始图像数据(未解压)
void eagle_praseLine(uint8_t *img_buffer,uint8_t *line_buffer,uint16_t line_num)
{
	uint16_t w;
	uint8_t *praw;
	praw = img_buffer + line_num*(CAMERA_W/8);		//指向解析行	

	for(w=0;w<(CAMERA_W/8);w++)
	{
#if EAGLE_LEFT_MSB == 1		//高位在左
		*line_buffer++= ((*praw&0x80)==0)?0:0xff;
		*line_buffer++= ((*praw&0x40)==0)?0:0xff;
		*line_buffer++= ((*praw&0x20)==0)?0:0xff;
		*line_buffer++= ((*praw&0x10)==0)?0:0xff;
		*line_buffer++= ((*praw&0x08)==0)?0:0xff;
		*line_buffer++= ((*praw&0x04)==0)?0:0xff;
		*line_buffer++= ((*praw&0x02)==0)?0:0xff;
		*line_buffer++= ((*praw&0x01)==0)?0:0xff;		
#else//低位在左	
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


