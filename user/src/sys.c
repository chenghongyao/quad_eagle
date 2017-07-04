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



void tim3_capture_init()
{
	TIM_ICInitTypeDef TIM_ICStructure;
//����TIM3_CH4�����ز���
	H_TIMx_ENRCC(3);
	H_TIMEBASE_Init(3,1,1000);	//1ms����
	
	TIM_ICStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICStructure.TIM_ICFilter = 0;		//0-F
	TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//�����ز���
	TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;				//����Ƶ
	TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//ͨ������ѡ��
	TIM_ICInit(TIM3,&TIM_ICStructure);
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
	H_GPIO_Init(B,7,GPIO_Speed_50MHz,GPIO_Mode_Out_PP); //BEEP
	beepM = 0;
	ledM = 1;
	
	
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
	H_GPIO_Init(B,10,GPIO_Speed_50MHz,GPIO_Mode_IPU);	 	//���ж�
	H_EXTI_Init(B,10, EXTI_Trigger_Rising_Falling,ENABLE);//���ж�������,�½�����������,�����ز������
	H_EXTI_IT_DISABLE(10);	
	H_NVIC_INIT(EXTI15_10,0,0);
	
	
	tim3_capture_init();	//TIIM3_CH4�½������ش���(DMA)	
	H_RCC_ENAHB(DMA1);					//DMA��ʼ��
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

//���ж�
void EXTI15_10_IRQHandler(void)
{
	if(H_EXTI_ISPEND_F(10))
	{
		if(EAGLE_VSYNC == 1)	//������,һ֡����
		{
			if(meagle.status == EAGLE_GATHERING) //�ɼ�����,��ֹδ�ɼ��Ϳ�ʼ���յ������ź�
			{
				//debug("%d\r\n",DMA_GetCurrDataCounter(DMA1_Channel3));//��ȡʣ��Ĳ�������
				meagle.status = EAGLE_FINISH;
				eagle_pauseCapture();
			}
		}
		else				//�½�����������
		{
			if(meagle.status == EAGLE_START)		//��������
			{
				meagle.cnt = 0;
				TIM_ClearFlag(TIM3,TIM_FLAG_CC4);										//���������ʱ��������־λ
				TIM_DMACmd(TIM3,TIM_DMA_CC4,ENABLE);								//������ʱ������
				DMA_Cmd(DMA1_Channel3,ENABLE);											//����DMA
				meagle.status = EAGLE_GATHERING;
			}
			else		//�ɼ�����
			{
				meagle.status = EAGLE_ERROR;
				eagle_pauseCapture();
			}
		}
		H_EXTI_CLEARPEND_F(10);
	}
}

