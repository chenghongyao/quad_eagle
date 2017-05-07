#include "adc.h"
#include "sys.h"





void adc1_init()
{

	//ADCʱ��
	H_RCC_ENAPB2(ADC1);			//ʹ��ʱ��
	H_RCC_RSTPAPB2(ADC1);		//��λ����

	RCC->CFGR &= ~RCC_CFGR_ADCPRE;		//��Ƶ����
	RCC->CFGR |= ADC_PRE_6;						//72M 6��Ƶ��12M
	
	
	//����ģʽ
	ADC1->CR1 &= ~ADC_CR1_DUALMOD;	//����ģʽ����,��������ģʽ
	ADC1->CR1 &= ~ADC_CR1_SCAN;		//�ر�ɨ��ģʽ	
	ADC1->CR2 &= ~ADC_CR2_CONT;		//����ת��
	ADC1->CR2 |= ADC_CR2_EXTTRIG;	//����ͨ��ʹ���ⲿ����
	ADC1->CR2 |= ADC_CR2_EXTSEL;	//���������ʽ
	ADC1->CR2 &= ~ADC_CR2_ALIGN;	//�����Ҷ���
	
	//���ù���ͨ������ 1��ͨ��
	ADC1->SQR1 &= ADC_SQR1_L;
	ADC1->SQR1 |= 0<<20;			//0����1��ת������

	//���ò���ʱ��
	//ADC1->SMPR2 &= ~(ADC_SMPCYCLE<<(ͨ����*3)); 0-9
	ADC1->SMPR2 &= ~(ADC_SMPCYCLE<<(3*3));				//���� 
	ADC1->SMPR2 |= ADC_SMPCYCLE_239_5<<(3*3);		 //���ò�������
	//������Դ
	ADC1->CR2 |= ADC_CR2_ADON;	
	//adcУ׼
	ADC1->CR2 |= ADC_CR2_RSTCAL;		//��ʼ��У׼�Ĵ���
	while(ADC1->CR2&ADC_CR2_RSTCAL);//��ʼ����ɺ���Ӳ������
	ADC1->CR2 |= ADC_CR2_CAL;		//��ʼУ׼
	while(ADC1->CR2&ADC_CR2_CAL);//��ɺ���Ӳ������
}



uint16_t adc1_getAdc(uint8_t ch)
{
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= ch;
	
	ADC1->CR2 |= ADC_CR2_SWSTART;	//����ת��
	while(!(ADC1->SR&ADC_SR_EOC));
	return  ADC1->DR;
}



uint16_t adc1_getAverage(uint8_t ch,uint8_t times)
{
	uint32_t temp=0;
	uint8_t i;
	for(i=0;i<times;i++)
		temp += adc1_getAdc(ch);
	return temp/times;
}


//�����ڲ��ο���ѹ��ȡ��ѹ*100
uint16_t adc1_getVoltage(uint8_t ch)
{
	uint32_t adc_x = adc1_getAdc(ch);
	uint16_t adc_ref = adc1_getAdc(17);
	return (adc_x*VOLTAGE_VREF)/adc_ref;
	
}

