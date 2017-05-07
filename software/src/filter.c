#include "filter.h"
#include "sys.h"
#include "math.h"
#include "cmath.h"




/******************************************************************************* 
功能：设计二阶巴特沃兹滤波器
参数：
filter:
fs:采样率
fc:截止频率
返回: 无
时间：1/20/2017
注意： 
*******************************************************************************/  
void lpf_butter(lpf2_t *filter, float fs, float fc)
{
	float fr = 0;
	float ohm = 0;
	float c = 0;

	fr = fs / fc;
	ohm = tanf(MY_PI/ fr);
	c = 1.0f + 2.0f*cosf(MY_PI / 4.0f)*ohm + ohm*ohm;

	if (fc > 0.0f)
	{
		filter->b0 = ohm*ohm / c;
		filter->b1 = 2.0f*filter->b0;
		filter->b2 = filter->b0;;
		filter->a1 = 2.0f*(ohm*ohm - 1.0f) / c;
		filter->a2 = (1.0f - 2.0f*cosf(MY_PI / 4.0f)*ohm + ohm*ohm) / c;

		filter->w1 = filter->w2 = 0.0f;
	}
}



//二阶IIR 直接II型实现
float lpf2nd_next(lpf2_t *filter, float xn)
{
	float yn, w0;
	w0 = xn - filter->w1 * filter->a1 - filter->w2 * filter->a2;
	if (isnan(w0) || isinf(w0))
	{
		w0 = xn;
	}
	yn = w0 * filter->b0 + filter->w1 * filter->b1 + filter->w2 * filter->b2;
	filter->w2 = filter->w1;
	filter->w1 = w0;
	return yn;
}


void lpf2nd_init(lpf2_t *filter,float b0,float b1,float b2,float a1,float a2)
{
	filter->b0 = b0;
	filter->b1 = b1;
	filter->b2 = b2;
	filter->a1 = a1;
	filter->a2 = a2;
	
	filter->w1 = 0.0f;
	filter->w2 = 0.0f;

	
}



//IIR 直接二型实现
float lpf_next(filter_t *filter, float xn)
{
	float yn;
	u8 i;
	u8 end = filter->order;

	filter->wn[0] = xn;
	yn = 0.0f;
	for (i = 1; i <= end; i++)
	{
		filter->wn[0] -= filter->wn[i] * filter->an[i];
	}

	if (isnan(filter->wn[0]) || isinf(filter->wn[0]))
	{
		filter->wn[0] = xn;
	}

	for (i = 0; i <= end; i++)
	{
		yn += filter->wn[i] * filter->bn[i];
	}

	for (i = end; i > 0; i--)
	{
		filter->wn[i] = filter->wn[i - 1];
	}
	return yn;
}

float filter_window(float xn,float *win ,uint16_t *index,uint16_t size)
{
	
	uint16_t i;
	double sum = 0.0f;
	win[*index] = xn;
	
	
	for (i = 0; i < size ; i++)
	{
		sum += *win;
		win++;
	}
	
	*index = *index + 1;
	if (*index >= size)
	{
		*index = 0;
	}
	return sum / size;
}
