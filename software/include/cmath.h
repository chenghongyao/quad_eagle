#ifndef _CMATH_H
#define _CMATH_H
#include "sys.h"

#define MY_PI 3.1415926f
#define MY_ISZERO(_d)	(((_d)>-0.001f)&&((_d)<0.001f))
#define MY_POWER(_d)	((_d)*(_d))
#define ABS(x) ( (x)>0?(x):-(x) )
#define MAX(_x,_y) ((_x)>(_y)?(_x):(_y))
#define MIN(_x,_y) ((_x)<(_y)?(_x):(_y))


#define DEG2RAD(_deg)	((_deg)*(MY_PI/180.0f))
#define RAD2DEG(_rad)	((_rad)*(180.0f/MY_PI))

#define RANGEVALUE(_val,_min,_max)	((_val)>(_max)?(_max):((_val)<(_min)?(_min):(_val)))
#define RANGEMAXVALUE(_val,_max)	((_val)>(_max)?(_max):(_val))
#define RANGEMINVALUE(_val,_min)	((_val)<(_min)?(_min):(_val))


//矩阵
void matrix_mv(float *ovec, float *mat, float *vec, uint8_t row, uint8_t col);
void matrix_dot(float *omat, float *mat1, float *mat2, uint8_t row1, uint8_t col1, uint8_t col2);
void matrix_transpose(float *omat, float *mat, uint8_t row, uint8_t col);
void matrix_eye(float *omat, uint8_t n);
float matrix_inv3b3(float *omat, float *mat);
float matrix_det2b2(float *mat);
float matrix_det3b3(float *mat);
float matrix_det4b4(float *mat);
//向量
void vector_cross(float *ovec, float *vec1, float *vec2);
void vector_add(float *ovec, float *vec1, float *vec2, uint8_t dim);
void vector_subtract(float *ovec, float *minuend, float *subtrahend, uint8_t dim);
void vector_scale(float *ovec, float *vec, float scale, uint8_t dim);
void vector_normalized(float *vec, uint8_t dim);
//其他
float invSqrt(float number);
void rad2deg(float *o, float *i, uint16_t size);
void deg2rad(float *o, float *i, uint16_t size);
float mywrap180(float x);
#endif
