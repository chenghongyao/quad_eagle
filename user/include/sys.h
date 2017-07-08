#ifndef __SYS_H
#define __SYS_H	  

#include <stm32f10x.h>   
#include "lib_helper.h"
#include "iap.h"
#include "myiic.h"
#include "sccb.h"
#include "flash.h"
#include "table.h"
#include "stdio.h"
#include "delay.h"



#include "nrf.h"
#include "esp8266.h"
#include "mpu6050.h"
#include "usart.h"
#include "motor.h"
#include "led.h"
#include "ultrasound.h"
#include "ms5611.h"
#include "hmc5883.h"
#include "eagle.h"
//#include "bytes.h"
#include "wos.h"
//#include "cmath.h"
//#include "controller.h"
//#include "ano.h"
//#include "imu.h"
//#include "rc.h"
//#include "flight.h"
//#include "mat.h"
//#include "filter.h"
//#include "ekf.h"
//#include "quat.h"
//#include "pid.h"

//#define _DEBUG
#define _LOGT			//����ʱ���ǩ


#ifdef _DEBUG
	#define debug(fmt,...) 	printf(fmt,##__VA_ARGS__)
#else
	#define debug(fmt,...)		do{}while(0)
#endif
		
		
#ifdef _LOGT			//ʱ���ǩ
		#define logt(msg,_us) printf("%s:us",_us)
#else
	#define logt(msg,_us)		do{}while(0)
#endif
		
		
		
		
#define NVIC_GROUP	2		//NVIC����
//Ex_NVIC_Configר�ö���
#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6 		
#define FTIR   1  //�½��ش���
#define RTIR   2  //�����ش���		
		
		
		
		
//JTAGģʽ���ö���
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	



//�ⲿ�ж�
#define H_EXTI_ISPEND_F(_line)		(EXTI->PR&EXTI_PR_PR##_line)
#define H_EXTI_CLEARPEND_F(_line)	do{(EXTI->PR |= EXTI_PR_PR##_line);}while(0)






//board hardware
//LED
#define ledM PCout(13)

#define beepM	PBout(7)

#define keyUp 	PBin(8)
#define keyDown 	PBin(9)


//NRF24L01
#define NRF_IRQ PAin(8)
#define NRF_CSN PBout(12)
#define NRF_CE PAout(11)
#define NRF_ReadWriteByte(_c) H_SPI_RWByte(2,_c)

//MOTOR
#define MOTOR_TR (TIM3->CCR1)
#define MOTOR_TL (TIM3->CCR2)
#define MOTOR_HL (TIM3->CCR3)
#define MOTOR_HR (TIM3->CCR4)	



//ultrasound
#define ULTRA_TRIG	PBout(9)
#define ULTRA_ECHO	PBin(8)





 
 
 
 
//EAGLE
#define EAGLE_VSYNC	PBin(10)					//���ж��ź�

//SCCB
#define SCCB_SDA  PBout(6)            
#define SCCB_SDAI PBin(6)
#define SCCB_SDA_OUT()			do{H_GPIO_SETMODEL_F(B,6,F_GPIO_Speed_50M,F_GPIO_Mode_Out_PP);}while(0)
#define SCCB_SDA_IN()				do{H_GPIO_SETMODEL_F(B,6,F_GPIO_Speed_IN,F_GPIO_Mode_IPU);}while(0)

#define SCCB_SCL PBout(5)
#define SCCB_DATA  					(GPIO_ReadInputData(GPIOA)&0x00FF)
 


void Sys_Soft_Reset(void);      //ϵͳ��λ
void Sys_Standby(void);         //����ģʽ 	
void JTAG_Set(uint8_t mode);
void MSR_MSP(u32 addr);	//���ö�ջ��ַ

void board_setup(void);


#endif














