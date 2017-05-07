#include "adc.h"
#include "sys.h"





void adc1_init()
{

	//ADC时钟
	H_RCC_ENAPB2(ADC1);			//使能时钟
	H_RCC_RSTPAPB2(ADC1);		//复位外设

	RCC->CFGR &= ~RCC_CFGR_ADCPRE;		//分频清零
	RCC->CFGR |= ADC_PRE_6;						//72M 6分频的12M
	
	
	//工作模式
	ADC1->CR1 &= ~ADC_CR1_DUALMOD;	//工作模式清零,独立工作模式
	ADC1->CR1 &= ~ADC_CR1_SCAN;		//关闭扫描模式	
	ADC1->CR2 &= ~ADC_CR2_CONT;		//单次转换
	ADC1->CR2 |= ADC_CR2_EXTTRIG;	//规则通道使用外部触发
	ADC1->CR2 |= ADC_CR2_EXTSEL;	//软件触发方式
	ADC1->CR2 &= ~ADC_CR2_ALIGN;	//数据右对齐
	
	//设置规则通道序列 1个通道
	ADC1->SQR1 &= ADC_SQR1_L;
	ADC1->SQR1 |= 0<<20;			//0代表1个转化序列

	//设置采样时间
	//ADC1->SMPR2 &= ~(ADC_SMPCYCLE<<(通道号*3)); 0-9
	ADC1->SMPR2 &= ~(ADC_SMPCYCLE<<(3*3));				//清零 
	ADC1->SMPR2 |= ADC_SMPCYCLE_239_5<<(3*3);		 //设置采样周期
	//开启电源
	ADC1->CR2 |= ADC_CR2_ADON;	
	//adc校准
	ADC1->CR2 |= ADC_CR2_RSTCAL;		//初始化校准寄存器
	while(ADC1->CR2&ADC_CR2_RSTCAL);//初始化完成后由硬件清零
	ADC1->CR2 |= ADC_CR2_CAL;		//开始校准
	while(ADC1->CR2&ADC_CR2_CAL);//完成后由硬件清零
}



uint16_t adc1_getAdc(uint8_t ch)
{
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= ch;
	
	ADC1->CR2 |= ADC_CR2_SWSTART;	//启动转换
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


//根据内部参考电压获取电压*100
uint16_t adc1_getVoltage(uint8_t ch)
{
	uint32_t adc_x = adc1_getAdc(ch);
	uint16_t adc_ref = adc1_getAdc(17);
	return (adc_x*VOLTAGE_VREF)/adc_ref;
	
}

