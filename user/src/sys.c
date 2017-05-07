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



extern volatile uint8_t eagle_buffer[CAMERA_SIZE];

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
	
	
	H_GPIO_Init(B,1,GPIO_Speed_50MHz,GPIO_Mode_IPD);	 	//����ʱ��
	H_EXTI_Init(B,1, EXTI_Trigger_Rising);	
	
	delay_init();
}


volatile uint16_t mcnt;
void EXTI15_10_IRQHandler(void)
{
	if(H_EXTI_ISPEND_F(15))
	{
		if(meagle.fStart == 1)
		{
			meagle.cnt = 0;
			mcnt = 0;
			EXTI_ClearFlag(EXTI_Line1);
			H_EXTI_IT_ENABLE(1,0,1);			
			meagle.fStart = 0;
		}
		else
		{
			H_EXTI_IT_DISABLE(1);	
			printf("%d,%2x\r\n",mcnt,eagle_buffer[2399]);
			meagle.hasUpdate = 1;
			eagle_pauseCapture();
		}

		H_EXTI_CLEARPEND_F(15);
	}
}




void EXTI1_IRQHandler(void)
{
	if(H_EXTI_ISPEND_F(1))
	{
		if(mcnt != 0)
		{
			eagle_buffer[mcnt-1] = GPIOA->IDR;
			
		}
			mcnt++;
			meagle.cnt++;
		H_EXTI_CLEARPEND_F(1);
	}
}