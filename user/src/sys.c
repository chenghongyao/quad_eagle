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
//�������ģʽ	  
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//ʹ��SLEEPDEEPλ (SYS->CTRL)	   
  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
 	PWR->CSR|=1<<8;          //����WKUP���ڻ���
	PWR->CR|=1<<2;           //���Wake-up ��־
	PWR->CR|=1<<1;           //PDDS��λ		
	__wfi();	 
}	     
//ϵͳ��λ   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(uint32_t)0x04;	  
} 		 
//JTAGģʽ����,��������JTAG��ģʽ
//mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;	   
//#define JTAG_SWD_DISABLE   0X02
//#define SWD_ENABLE         0X01
//#define JTAG_SWD_ENABLE    0X00		  
void JTAG_Set(uint8_t mode)
{
	uint32_t temp;
	temp=mode;
	temp<<=25;
	H_RCC_ENAPB2(AFIO);		//ʹ�ܸ���IOʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ
}


extern uint8_t eagle_buffer[CAMERA_SIZE];
void dma1_init()
{
	DMA_InitTypeDef DMA_InitStructure;
	/****����DMA****************/
	H_RCC_ENAHB(DMA1);					//��ʱ��
	DMA_DeInit(DMA1_Channel3); 
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&GPIOA->IDR);					//�����ַ PA��
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		  //8λ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//�����ַ����
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)eagle_buffer;;							//�ڴ��ַ
	DMA_InitStructure.DMA_BufferSize = CAMERA_SIZE;											//�ڴ��С
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;							//���ݿ��
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//�ڴ��ַ����ʹ��
	
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;													//�������赽�ڴ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 															//��������
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;											//�����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																//�ڴ浽�ڴ�	
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);																	//TIM3_CH4��ͨ��3
}

void tim3_capture_init()
{
	TIM_ICInitTypeDef TIM_ICStructure;
//����TIM3_CH4�����ز���
	H_TIMx_ENRCC(3);
	H_TIMEBASE_Init(3,1,1000);	//1ms����
	
	TIM_ICStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICStructure.TIM_ICFilter = 0;		//0-F
	TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;				//����Ƶ
	TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//ͨ������ѡ��
	TIM_ICInit(TIM3,&TIM_ICStructure);
	TIM_DMACmd(TIM3,TIM_DMA_CC4,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
}



//�ж�,����,����,ϵͳģ��
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
	H_GPIO_Init(C,15,GPIO_Speed_50MHz,GPIO_Mode_IPU);	 	//���ж�
	H_EXTI_Init(C,15, EXTI_Trigger_Falling);		
	
	
	//	H_GPIO_Init(B,1,GPIO_Speed_50MHz,GPIO_Mode_IPD);	 	//����ʱ��
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

