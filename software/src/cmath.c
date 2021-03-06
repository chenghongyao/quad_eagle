#include "cmath.h"
#include "sys.h"



//矩阵==================================================
//矩阵*列向量
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


//矩阵乘法
void matrix_dot(float *omat, float *mat1, float *mat2, u8 row1, u8 col1, u8 col2)
{
	u8 r, c, t;
	float *p1,*p2,*p1start;
	
	p1start = mat1;
	for (r = 0; r < row1; r++,p1start += col1)			//结果行
	{
		for (c = 0; c < col2; c++,omat++)		//结果列
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


//矩阵转置
void matrix_transpose(float *omat, float *mat,u8 row, u8 col)
{
	u8 r, c;
	float *p2;
	for ( r = 0; r < row; r++)	//原矩阵的每一行
	{
		p2 = omat + r;
		for (c = 0;	c < col; c++,mat++,p2+=row)	//改为每一列
		{
			*p2 = *mat;
	
		}
	}
}


//单位矩阵
void matrix_eye(float *omat,u8 n)
{
	u8 i, j;
	for (i = 0; i < n; i++)		//每一行
	{
		for (j = 0; j < n; j++)	//所有元素
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

//3*3矩阵求逆,返回行列式值,为0时矩阵不可逆
//乘法:30
//除法:1
//加减:11+9
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

//部分主元的高斯消去法求逆矩阵
//矩阵不可逆返回0
// u8 matrix_inv(float *mat, float *omat,u8 n)
// {
// 	u8 i, j, k;
// 	float max,absmax,temp;
// 	float t[MATRIX_MAX_ORDER*MATRIX_MAX_ORDER];                //临时矩阵  
// 	u8 c;				//行迭代
// 
// 	u8 m;							//主元对应位置,第i行i列,i*n+i
// 	int currow;						//当前行头 i*n
// 
// 	array_copy(t, mat,n*n);					//将mat矩阵存放在临时矩阵t[n][n]中  
// 	eye(omat, n);
// 	for (i = 0; i < n; i++)					//对于每一列
// 	{
// 		currow = i * n;
// 		m = currow + i;					//第i行i列坐标
// 		//寻找主元  
// 		max = t[m];					//最大主元数
// 		absmax = fabs(max);
// 		k = i;						//最大主元所在行
// 		c = currow+n;					//下一行
// 		for (j = i + 1; j < n; j++)			//对于该列的每行
// 		{
// 			if (fabs(t[c + i]) > absmax) 		//求最大主元所在行
// 			{
// 				max = t[c + i];
// 				k = j;				//最大主元行修改
// 			}
// 			c += n;
// 		}
// 		if (max == 0)return 0;					//判断主元是否为0, 若是, 则矩阵mat不是满秩矩阵,不存在逆矩阵  
// 			
// 		//如果主元所在行不是第i行，进行行交换  
// 		if (k != i)
// 		{
// 			u8 d2 = k * n;		//最大主元行
// 
// 			for (j = 0; j < n; j++)		//对于两行的每一列
// 			{
// 				u8 c1 = currow + j;
// 				u8 c2 = d2 + j;
// 
// 				temp = t[c1];
// 				t[c1] = t[c2];
// 				t[c2] = temp;
// 				//omat伴随交换  
// 				temp = omat[c1];
// 				omat[c1] = omat[c2];
// 				omat[c2] = temp;
// 			}
// 		}
// 		
// 		//消去mat的第i列除去i行以外的各行元素  
// 		temp = 1 / max;
// 		for (j = 0; j < n; j++)					//对于每一行
// 		{
// 			u8 c1 = currow + j;
// 			t[c1] *= temp;        				//主对角线上的元素变为1  
// 			omat[c1] *= temp;        			//伴随计算  
// 		}
// 
// 		c = 0;
// 		for (j = 0; j < n; j++)        //第0行->第n行  其他行减去主元行
// 		{
// 			if (j != i)                	//不是第i行 
// 			{
// 				temp = t[c + i];
// 				for (k = 0; k < n; k++)   //第j行元素 - i行元素*j列i行元素  
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




//矩阵行列式
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


//=======向量===================================

//叉乘3*3
void vector_cross(float *ovec,float *vec1,float *vec2)
{
	ovec[0] = vec1[1]*vec2[2]-vec1[2]*vec2[1];
	ovec[1] = vec1[2]*vec2[0]-vec1[0]*vec2[2];
	ovec[2] = vec1[0]*vec2[1]-vec1[1]*vec2[0];
}

/******************************************************************************* 
功能：向量加法
参数：
ovec:结果向量,为NULL时结果存放到vec1中
..
返回: 无
时间：1/20/2017
注意： 
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
功能：向量减法:ovec = minuend - subtrahend;
参数：
ovec:结果向量,为NULL时结果存放到minuend中
minuend:被减数
subtrahend:减数
dim:维数
返回: 无
时间：1/20/2017
注意： 
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


//比例 ovec为空时结果存放到vec中
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
	sum = invSqrt(sum);	//平方根倒数
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
