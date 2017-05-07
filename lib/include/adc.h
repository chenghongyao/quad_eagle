#ifndef	_ADC_H
#define _ADC_H

#include "sys.h"

//Ԥ��Ƶϵ��
#define ADC_PRE_2 0x00
#define ADC_PRE_4	RCC_CFGR_ADCPRE_0
#define ADC_PRE_6 RCC_CFGR_ADCPRE_1
#define ADC_PRE_8 (RCC_CFGR_ADCPRE_0|RCC_CFGR_ADCPRE_1)


//��������
#define ADC_SMPCYCLE				0x07		//
#define ADC_SMPCYCLE_1_5 		0x00
#define ADC_SMPCYCLE_7_5 		0x01
#define ADC_SMPCYCLE_13_5 	0x02
#define ADC_SMPCYCLE_28_5 	0x03
#define ADC_SMPCYCLE_41_5 	0x04
#define ADC_SMPCYCLE_55_5 	0x05
#define ADC_SMPCYCLE_71_5 	0x06			//
#define ADC_SMPCYCLE_239_5 	0x07			//21us@12M




#define VOLTAGE_VREF 123					//�ο���ѹ*100
/*
1.�޸�IO������
2.�޸Ĳ���ʱ�䶨���ͨ��
*/

void adc1_init(void);
uint16_t adc1_getAdc(uint8_t ch);
uint16_t adc1_getAverage(uint8_t ch,uint8_t times);
uint16_t adc1_getVoltage(uint8_t ch);

#endif

