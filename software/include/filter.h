#ifndef _FILTER_H
#define _FILTER_H
#include "sys.h"

#define FLITER_LPF_MAXORDER 10


//yn = y(n-1) + FILTER1ST_COFF(fc,dt)*[xn-y(n-1)]
#define FILTER1ST_COEF(fc,dt) (1.0f/(1.0f+1.0f/(2.0f*3.14*dt*fc)))

typedef struct
{
	float an[FLITER_LPF_MAXORDER];
	float bn[FLITER_LPF_MAXORDER];
	float wn[FLITER_LPF_MAXORDER];
	u8 order;
}filter_t;

typedef struct
{
	float w1;
	float w2;
	float a1;
	float a2;

	float b0;
	float b1;
	float b2;
}lpf2_t;

void lpf_butter(lpf2_t *filter, float fs, float fc);
void lpf2nd_init(lpf2_t *filter, float b0, float b1, float b2, float a1, float a2);
float lpf2nd_next(lpf2_t *filter, float xn);
float lpf_next(filter_t *filter, float xn);

float filter_window(float xn, float *win, uint16_t *index, uint16_t size);
#endif
