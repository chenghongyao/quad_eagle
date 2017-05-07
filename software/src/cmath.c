#include "cmath.h"
#include "sys.h"



//����==================================================
//����*������
void matrix_mv(float *ovec, float *mat, float *vec, u8 row, u8 col)
{
	u8 r, c;
	float *p;
	for (r = 0; r < row; r++)
	{
		*ovec = 0;
		p = vec;
		for (c = 0; c < col; c++,mat++,p++)
		{
			*ovec += *mat * (*p);
		}
		ovec++;
	}
}


//����˷�
void matrix_dot(float *omat, float *mat1, float *mat2, u8 row1, u8 col1, u8 col2)
{
	u8 r, c, t;
	float *p1,*p2,*p1start;
	
	p1start = mat1;
	for (r = 0; r < row1; r++,p1start += col1)			//�����
	{
		for (c = 0; c < col2; c++,omat++)		//�����
		{
			p1 = p1start;
			p2 = mat2+c;	
			*omat = 0;
			for (t = 0; t < col1; t++,p1++,p2+=col2)
			{
				*omat += *p1 * (*p2);
			}
		}
	}
}


//����ת��
void matrix_transpose(float *omat, float *mat,u8 row, u8 col)
{
	u8 r, c;
	float *p2;
	for ( r = 0; r < row; r++)	//ԭ�����ÿһ��
	{
		p2 = omat + r;
		for (c = 0;	c < col; c++,mat++,p2+=row)	//��Ϊÿһ��
		{
			*p2 = *mat;
	
		}
	}
}


//��λ����
void matrix_eye(float *omat,u8 n)
{
	u8 i, j;
	for (i = 0; i < n; i++)		//ÿһ��
	{
		for (j = 0; j < n; j++)	//����Ԫ��
		{
			*omat++ = (i == j) ? 1.0f : 0.0f;
		}
	}
}

void matrix_copy(float *omat,float *mat,uint16_t row_start,uint16_t row_end,uint16_t col_start,uint16_t col_end,uint16_t row,uint16_t col)
{
	float *pr;
	float *pw = omat;
	for (; row_start < row_end; row_start++)
	{
		pr = mat + row_start*col + col_start;
		for (; col_start < col_end; col_start++)
		{
			*pw++ = *pr++;

		}
	}
}
float matrix_inv2b2(float *omat,float *mat)
{
	float det;
	float invdet;
	det = mat[0] * mat[3] - mat[1] * mat[2];
	if(MY_ISZERO(det))
		return 0.0f;

	invdet = 1.0f / det;
	omat[0] = mat[3] * invdet;
	omat[1] = -mat[1]*invdet;
	omat[2] = -mat[2]*invdet;
	omat[3] = mat[0]*invdet;
	return det;
}

//3*3��������,��������ʽֵ,Ϊ0ʱ���󲻿���
//�˷�:30
//����:1
//�Ӽ�:11+9
float matrix_inv3b3(float *omat, float *mat)
{
	float det;
	omat[0] = mat[4] * mat[8] - mat[5] * mat[7];
	omat[3] = mat[5] * mat[6] - mat[3] * mat[8];
	omat[6] = mat[3] * mat[7] - mat[4] * mat[6];

	det = mat[0] * omat[0] + mat[1] * omat[3] + mat[2] * omat[6];

	if(MY_ISZERO(det))
		return 0.0f;
	
	omat[1] = mat[2] * mat[7] - mat[1] * mat[8];
	omat[4] = mat[0] * mat[8] - mat[2] * mat[6];
	omat[7] = mat[1] * mat[6] - mat[0] * mat[7];
	omat[2] = mat[1] * mat[5] - mat[2] * mat[4];
	omat[5] = mat[2] * mat[3] - mat[0] * mat[5];
	omat[8] = mat[0] * mat[4] - mat[1] * mat[3];
	vector_scale(NULL, omat, 1.0f / det, 9);
	return det;
}

//������Ԫ�ĸ�˹��ȥ���������
//���󲻿��淵��0
// u8 matrix_inv(float *mat, float *omat,u8 n)
// {
// 	u8 i, j, k;
// 	float max,absmax,temp;
// 	float t[MATRIX_MAX_ORDER*MATRIX_MAX_ORDER];                //��ʱ����  
// 	u8 c;				//�е���
// 
// 	u8 m;							//��Ԫ��Ӧλ��,��i��i��,i*n+i
// 	int currow;						//��ǰ��ͷ i*n
// 
// 	array_copy(t, mat,n*n);					//��mat����������ʱ����t[n][n]��  
// 	eye(omat, n);
// 	for (i = 0; i < n; i++)					//����ÿһ��
// 	{
// 		currow = i * n;
// 		m = currow + i;					//��i��i������
// 		//Ѱ����Ԫ  
// 		max = t[m];					//�����Ԫ��
// 		absmax = fabs(max);
// 		k = i;						//�����Ԫ������
// 		c = currow+n;					//��һ��
// 		for (j = i + 1; j < n; j++)			//���ڸ��е�ÿ��
// 		{
// 			if (fabs(t[c + i]) > absmax) 		//�������Ԫ������
// 			{
// 				max = t[c + i];
// 				k = j;				//�����Ԫ���޸�
// 			}
// 			c += n;
// 		}
// 		if (max == 0)return 0;					//�ж���Ԫ�Ƿ�Ϊ0, ����, �����mat�������Ⱦ���,�����������  
// 			
// 		//�����Ԫ�����в��ǵ�i�У������н���  
// 		if (k != i)
// 		{
// 			u8 d2 = k * n;		//�����Ԫ��
// 
// 			for (j = 0; j < n; j++)		//�������е�ÿһ��
// 			{
// 				u8 c1 = currow + j;
// 				u8 c2 = d2 + j;
// 
// 				temp = t[c1];
// 				t[c1] = t[c2];
// 				t[c2] = temp;
// 				//omat���潻��  
// 				temp = omat[c1];
// 				omat[c1] = omat[c2];
// 				omat[c2] = temp;
// 			}
// 		}
// 		
// 		//��ȥmat�ĵ�i�г�ȥi������ĸ���Ԫ��  
// 		temp = 1 / max;
// 		for (j = 0; j < n; j++)					//����ÿһ��
// 		{
// 			u8 c1 = currow + j;
// 			t[c1] *= temp;        				//���Խ����ϵ�Ԫ�ر�Ϊ1  
// 			omat[c1] *= temp;        			//�������  
// 		}
// 
// 		c = 0;
// 		for (j = 0; j < n; j++)        //��0��->��n��  �����м�ȥ��Ԫ��
// 		{
// 			if (j != i)                	//���ǵ�i�� 
// 			{
// 				temp = t[c + i];
// 				for (k = 0; k < n; k++)   //��j��Ԫ�� - i��Ԫ��*j��i��Ԫ��  
// 				{
// 					u8 c1 = c + k;
// 					u8 c2 = currow + k;
// 					t[c1] = t[c1] - t[c2] * temp;
// 					omat[c1] = omat[c1] - omat[c2] * temp;
// 				}
// 			}
// 			c += n;
// 		}
// 	}
// 	return 1;
// }




//��������ʽ
float matrix_det2b2(float *mat)
{
	return mat[0]*mat[3]-mat[1]*mat[2];
}

float matrix_det3b3(float *mat)
{
	float res = 0.0f;
	res += mat[0]*(mat[4]*mat[8]-mat[5]*mat[7]);
	res -= mat[1]*(mat[3]*mat[8]-mat[5]*mat[6]);
	res += mat[2]*(mat[3]*mat[7]-mat[4]*mat[6]);
	return res;
}

float matrix_det4b4(float *mat)
{
	float k1,k2,k3,k4,k5,k6,res;
	k1 = mat[8]*mat[13]-mat[9]*mat[12];
	k2 = mat[9]*mat[14]-mat[10]*mat[13];
	k3 = mat[10]*mat[15]-mat[11]*mat[14];
	
	k4 = mat[8]*mat[14]-mat[10]*mat[12];
	k5 = mat[8]*mat[15]-mat[11]*mat[12];
	k6 = mat[9]*mat[15]-mat[11]*mat[13];

	
	res = 0;
	res += mat[0]*(mat[5]*k3-mat[6]*k6+mat[7]*k2);
	res -= mat[1]*(mat[4]*k3-mat[6]*k5+mat[7]*k4);
	res += mat[2]*(mat[4]*k6-mat[5]*k5+mat[7]*k1);
	res -= mat[3]*(mat[4]*k2-mat[5]*k4+mat[6]*k1);
	return res;
}


//=======����===================================

//���3*3
void vector_cross(float *ovec,float *vec1,float *vec2)
{
	ovec[0] = vec1[1]*vec2[2]-vec1[2]*vec2[1];
	ovec[1] = vec1[2]*vec2[0]-vec1[0]*vec2[2];
	ovec[2] = vec1[0]*vec2[1]-vec1[1]*vec2[0];
}

/******************************************************************************* 
���ܣ������ӷ�
������
ovec:�������,ΪNULLʱ�����ŵ�vec1��
..
����: ��
ʱ�䣺1/20/2017
ע�⣺ 
*******************************************************************************/  
void vector_add(float *ovec,float *vec1,float *vec2,u8 dim)
{
	u8 i;
	if(ovec == NULL)
	{
		for(i=0;i<dim;i++,vec1++,vec2++)
			*vec1 += *vec2;
	}
	else
	{
		for(i=0;i<dim;i++,ovec++,vec1++,vec2++)
			*ovec = *vec1 + *vec2;
	}

}

/******************************************************************************* 
���ܣ���������:ovec = minuend - subtrahend;
������
ovec:�������,ΪNULLʱ�����ŵ�minuend��
minuend:������
subtrahend:����
dim:ά��
����: ��
ʱ�䣺1/20/2017
ע�⣺ 
*******************************************************************************/  
void vector_subtract(float *ovec,float *minuend,float *subtrahend,u8 dim)
{
	u8 i;
	if (ovec == NULL)
	{
		for (i = 0; i < dim; i++, minuend++, subtrahend++)
		{
			*minuend -= *subtrahend;
		}
	}
	else
	{
		for (i = 0; i < dim; i++, ovec++,minuend++, subtrahend++)
		{
			*ovec = *minuend - *subtrahend;
		}
		
	}
	
}


//���� ovecΪ��ʱ�����ŵ�vec��
void vector_scale(float *ovec,float *vec,float scale,u8 dim)
{
	u8 i;
	if(ovec == NULL)
	{
		for(i=0;i<dim;i++,vec++)
		{
			*vec *= scale;
		}
	}
	else
	{
		for(i=0;i<dim;i++,vec++,ovec++)
			*ovec = *vec * scale;
	}
}


void vector_normalized(float *vec,u8 dim)
{
	u8 i;
	float sum;
	for(i=0,sum=0.0f;i<dim;i++,vec++)
	{
		sum += *vec * (*vec);
	}
	sum = invSqrt(sum);	//ƽ��������
	for(i=0;i<dim;i++,vec++)
	{
		*vec *= sum;
	}
}

//////////////////////////////////////////////////////////////////////////
float invSqrt(float number)
{
	volatile long i;
	volatile float x, y;
	volatile const float f = 1.5F;

	x = number * 0.5F;
	y = number;
	i = *((long *)&y);
	i = 0x5f375a86 - (i >> 1);
	y = *((float *)&i);
	y = y * (f - (x * y * y));
	return y;
}


// 
void rad2deg(float *o, float *i, uint16_t size)
{
	uint16_t  t;
	for (t = 0; t < size ; t++,o++,i++)
	{
		*o = RAD2DEG(*i);
	}
}

void deg2rad(float *o,float *i,uint16_t size)
{
	uint16_t  t;
	for (t = 0; t < size; t++, o++, i++)
	{
		*o = DEG2RAD(*i);
	}
}

float mywrap180(float x)
{
	return (x>180.0f?(x-360.0f):(x<-180.0f?(x+360.0f):x));
}
