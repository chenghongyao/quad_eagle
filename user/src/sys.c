#include "sys.h" 
#include "usart.h"
#include "myiic.h"
#include "rc.h"

#include "ccd.h"
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//进入待机模式	  
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//使能SLEEPDEEP位 (SYS->CTRL)	   
  	RCC->APB1ENR|=1<<28;     //使能电源时钟	    
 	PWR->CSR|=1<<8;          //设置WKUP用于唤醒
	PWR->CR|=1<<2;           //清除Wake-up 标志
	PWR->CR|=1<<1;           //PDDS置位		
	__wfi();	 
}	     
//系统软复位   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(uint32_t)0x04;	  
} 		 
//JTAG模式设置,用于设置JTAG的模式
//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;	   
//#define JTAG_SWD_DISABLE   0X02
//#define SWD_ENABLE         0X01
//#define JTAG_SWD_ENABLE    0X00		  
void JTAG_Set(uint8_t mode)
{
	uint32_t temp;
	temp=mode;
	temp<<=25;
	H_RCC_ENAPB2(AFIO);		//使能复用IO时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
}



void tim3_capture_init()
{
	TIM_ICInitTypeDef TIM_ICStructure;
//配置TIM3_CH4上升沿捕获
	H_TIMx_ENRCC(3);
	H_TIMEBASE_Init(3,1,1000);	//1ms计数
	
	TIM_ICStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICStructure.TIM_ICFilter = 0;		//0-F
	TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;				//不分频
	TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//通道方向选择
	TIM_ICInit(TIM3,&TIM_ICStructure);
	TIM_Cmd(TIM3,ENABLE);
}



//中断,引脚,外设,系统模块
void board_setup(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0);
	JTAG_Set(SWD_ENABLE);
	
	//*LED*/
	H_GPIO_Init(C,13,GPIO_Speed_50MHz,GPIO_Mode_Out_PP); //LED
	H_GPIO_Init(B,7,GPIO_Speed_50MHz,GPIO_Mode_Out_PP); //BEEP
	beepM = 0;
	/*KEY*/
	H_GPIO_Init(B,8,GPIO_Speed_50MHz,GPIO_Mode_IPU); 
	H_GPIO_Init(B,9,GPIO_Speed_50MHz,GPIO_Mode_IPU); 
	
	/*USART1*/
	usart1_init();
	
	
	
		//NRF24L01 
	H_GPIO_Init(A,8,GPIO_Speed_50MHz,GPIO_Mode_IPU); 	//IRQ
	H_GPIO_Init(B,12,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);//CSN
	H_GPIO_Init(A,11,GPIO_Speed_50MHz,GPIO_Mode_Out_PP); //CE
	NRF_CE = 0;
	NRF_CSN = 1;
	
	/*SPI2 NRF24L01*/
	H_GPIO_Init(B,15,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);//MOSI
	H_GPIO_Init(B,14,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);//MISO
	H_GPIO_Init(B,13,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);//SCK
	
	H_SPIx_ENRCC(2);
	H_SPI_Init(2,SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_FirstBit_MSB,8);
	
	
	//EAGLE
	SCCB_Init();
	H_GPIO_Init(C,15,GPIO_Speed_50MHz,GPIO_Mode_IPU);	 	//场中断
	H_EXTI_Init(C,15, EXTI_Trigger_Falling,ENABLE);	
	H_EXTI_IT_DISABLE(15);	
	H_NVIC_INIT(EXTI15_10,0,0);
	
	
	
	tim3_capture_init();	//TIIM3_CH4上升沿触发
	
	H_RCC_ENAHB(DMA1);					//DMA初始化
	DMA_DeInit(DMA1_Channel3); 
	
	delay_init();
		/*TIM1 WOS*/
	H_TIM18_ENRCC(1);
	H_TIMEBASE_Init(1,72,1000);
	H_TIMEBASE18_IT_UPDATE(1,0,2);
	H_TIMEBASE_START(1);
}


//WOS
void TIM1_UP_IRQHandler(void)
{
	if (H_TIMEBASE_ISUPDATE_F(1))
	{
		wos_update();
		H_TIMEBASE_CLEARUPDATE_F(1);
	}
}


void EXTI15_10_IRQHandler(void)
{
	if(H_EXTI_ISPEND_F(15))
	{
		if(meagle.fStart == 1)//开始捕获图像
		{
			meagle.cnt = 0;
			TIM_ClearFlag(TIM3,TIM_FLAG_CC4);	//清除定时器标志
			DMA_Cmd(DMA1_Channel3,ENABLE);		//启动DMA
			meagle.fStart = 0;
		}
		else	//图像捕获完成
		{
			//printf("%d\r\n",meagle.cnt );
			debug("%d\r\n",DMA_GetCurrDataCounter(DMA1_Channel3));
			meagle.hasUpdate = 1;
			eagle_pauseCapture();
		}

		H_EXTI_CLEARPEND_F(15);
	}
}




//void EXTI1_IRQHandler(void)
//{
//	if(H_EXTI_ISPEND_F(1))
//	{
//		if(meagle.cnt != 0)
//		{
//			meagle.image[meagle.cnt-1] = GPIOA->IDR;
//		}
//		meagle.cnt++;
//		H_EXTI_CLEARPEND_F(1);
//	}
//}

