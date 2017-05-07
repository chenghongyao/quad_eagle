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


extern uint8_t eagle_buffer[CAMERA_SIZE];
void dma1_init()
{
	DMA_InitTypeDef DMA_InitStructure;
	/****配置DMA****************/
	H_RCC_ENAHB(DMA1);					//打开时钟
	DMA_DeInit(DMA1_Channel3); 
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&GPIOA->IDR);					//外设地址 PA口
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		  //8位
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设地址不变
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)eagle_buffer;;							//内存地址
	DMA_InitStructure.DMA_BufferSize = CAMERA_SIZE;											//内存大小
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;							//数据宽度
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//内存地址自增使能
	
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;													//单向，外设到内存
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 															//连续接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;											//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//内存到内存	
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);																	//TIM3_CH4在通道3
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
	TIM_DMACmd(TIM3,TIM_DMA_CC4,ENABLE);
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
	
	/*USART1*/
	usart1_init();
	SCCB_Init();
	H_GPIO_Init(C,15,GPIO_Speed_50MHz,GPIO_Mode_IPU);	 	//场中断
	H_EXTI_Init(C,15, EXTI_Trigger_Falling);		
	
	
	//	H_GPIO_Init(B,1,GPIO_Speed_50MHz,GPIO_Mode_IPD);	 	//像素时钟
//	H_EXTI_Init(B,1, EXTI_Trigger_Rising);	
	
	tim3_capture_init();
	dma1_init();

	
	delay_init();
}


void EXTI15_10_IRQHandler(void)
{
	if(H_EXTI_ISPEND_F(15))
	{
		if(meagle.fStart == 1)
		{
			meagle.cnt = 0;
			TIM_ClearFlag(TIM3,TIM_FLAG_CC4);
			DMA_Cmd(DMA1_Channel3,ENABLE);
			
//			EXTI_ClearFlag(EXTI_Line1);
//			H_EXTI_IT_ENABLE(1,0,1);			
			meagle.fStart = 0;
		}
		else
		{
//			H_EXTI_IT_DISABLE(1);	
			//printf("%d\r\n",meagle.cnt );
			printf("%d\r\n",DMA_GetCurrDataCounter(DMA1_Channel3));
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

