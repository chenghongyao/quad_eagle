#include "route.h"
#include "sys.h"
#include "csp.h"
#include "cmath.h"
#include "led.h"






















#define SCANWIDTH_IN_WHITE		5					//隔行在白线上的最大搜索范围
//img_buffer:原始图像数据(未解压)
//result:搜索结果,0为最靠近摄像头的一行
//linie_start:搜索起始行,倒序,行数递减
//line_count:解压line_count行,result需要有足够的大小
//search_start:第一行(line_start)的搜索起点(使用上一帧的结果)
//返回line_start行搜索结果
// line_count<=line_start+1

/***
待补充
起点为黑点时,向左右扫描
1.起点为白点时,向左右扫描,根据黑点的距离和宽度选择确定相信哪一个
2.当出现断线时,根据最近一个点的斜率生成插值
3.当两边黑点数目都过多时说明这是个十字口,当只有一边很多时说明这是一个直角拐弯
4.黑线不应该有限制,十字线应该以变化量为标准,突然变大了判断为十字
**/
uint16_t route_find(uint8_t *img_buffer,uint8_t *result,uint16_t line_start,uint16_t line_count,uint16_t search_start)
{
	uint16_t i,j,k;								
	uint8_t line_buffer[CAMERA_W];			//行解压缓存
	uint16_t scan_width;             	 	//扫描宽度         
	uint16_t right, left;              	//左右边界坐标
	uint16_t cnt = 0;			//返回数组索引
	
	uint16_t width_in_black = 50;
	uint16_t width_in_white = 70;
	
	for(i=0;i<line_count;i++)			
	{
		uint8_t fRight=0,fLeft= 0;				 //找到左右边界标记		
		eagle_praseLine(img_buffer,line_buffer,line_start+i);				//先解压得到行数据，从靠近摄像头开始
		if (line_buffer[search_start] == 0xff)											//起点为黑,左右边界在两边
		{
				//搜索左边界
				scan_width = (width_in_black < search_start) ? width_in_black : search_start; //扫描长度
				j = search_start - 1;                          									//向左扫描起点
				for (k = 0; k < scan_width; k++)    
				{
						if (line_buffer[j] == 0) 																		//黑->白,左边界
						{              
								left = j;
								fLeft = 1;
								break;                           
						}                       
						j--;				
				}
				
				if(fLeft)																											//搜索到了左边界才继续搜索右边界
				{
						scan_width = (width_in_black < (CAMERA_W - 1 - search_start)) ? width_in_black : (CAMERA_W - 1 - search_start); //扫描长度
						j = search_start + 1;                          						//向右扫描起点
						for (k = 0; k < scan_width; k++)    
						{
								if (line_buffer[j] == 0) 															//黑->白,右边界
								{
										right = j;
										fRight = 1;
										break;
								}
								j++;
						}
				}                   
		}
		else																																//起点为白点,左右边界在同一侧
		{
				//先向左搜索
				scan_width = (width_in_white < search_start) ? width_in_white : search_start; //扫描长度
				j = search_start - 1;                          									//向左扫描起点
				for (k = 0; k < scan_width; k++)   
				{                    
						if (line_buffer[j] == 0xff) 																//白->黑,右边界
						{
								right = j+1;
								fRight = 1;
								break;                
						}                        
						j--;
				}
				
				if(fRight)																										//找到了右边界才继续寻找左边界
				{
						scan_width = (width_in_black < j) ? width_in_black : j; //扫描长度
						j--;                           														//向左扫描起点
						for (k = 0; k < scan_width; k++)    
						{
								if (line_buffer[j] == 0) 															//黑->白,左边界
								{
										left = j;
										fLeft = 1;
										break;
								}
								j--;
						}
				}
				
				if (fLeft == 0) 																							//向左搜不到，向右搜索
				{
					scan_width = (width_in_white < (CAMERA_W - 1 - search_start)) ? width_in_white : (CAMERA_W - 1 - search_start); //扫描长度
					j = search_start + 1;                          																										//向右扫描起点
					for (k = 0; k < scan_width; k++)    
					{
							if (line_buffer[j] == 0xff)														//白->黑,左边界
							{
								left = j-1;
								fLeft = 1;
								break;
							}
							j++;
					}
					
					if(fLeft)//找到了左边界才继续找右边界
					{
							scan_width = (width_in_black < (CAMERA_W - 1 - j)) ? width_in_black : (CAMERA_W - 1 - j); //扫描长度
							j++; 																											//向右扫描起点
							for (k = 0; k < scan_width; k++)   
							{
									if (line_buffer[j]  == 0) //黑->白，右边界
									{
											right = j;
											fRight = 1;
											break;
									}
									j++;
							}
					}
				}
		}
		
	
		//计算结果
		if (!(fLeft ==1) && (fRight == 1))//不能找到两条边界,使用上一行的值
		{
				if (i == 0) result[cnt] = search_start;  		//使用上一帧的结果
				else    result[cnt] = result[cnt-1];			 //使用上一行的结果
		}
		else
		{
				result[cnt] = (right + left) / 2;
				width_in_black = (right-left)/2+4;		//根据当前宽度设置下一行黑线宽度
		}
		search_start = result[cnt];
		width_in_white = SCANWIDTH_IN_WHITE;		//剩下的线扫描宽度较小 
		cnt++;
	}
	
	return result[0];
}









float calculation_variance(float a,float b, float c,float average)
{
    float variance;
    variance = ((a - average)*(a - average)+(b - average)*(b - average)+(c - average)*(c - average))/3;
    return variance;
}

float calculation_average(float a,float b, float c)
{
	float average_value;
	average_value = (a + b + c)/3;
	return average_value;
}













/************************************
识别左拐角,有拐角,十字,圆(?)
左拐角:左边界变大,右边界不变
右拐角:右边界变大,左边界不变
十字角:左右边界变大
圆:左右边界越来越大,达到做大值后越来越小
*************************************/
uint16_t route_find2(uint8_t *img_buffer,uint8_t *result,uint16_t line_start,uint16_t line_count,uint16_t search_start)
{
	uint16_t i,j,k;								
	uint8_t line_buffer[CAMERA_W];													//行解压缓存
          											//扫描宽度         
	

	//uint16_t line_count = CAMERA_H-line_start;							//扫描行数
	uint16_t width_in_white;																//白线上的扫面宽度
	uint8_t fRight,fLeft;																		//找到左右边界标记		
	uint16_t right, left;              											//左右边界坐标
	uint16_t scan_width;																		//扫描宽度
	
	
	uint16_t width_left,width_right,width_line;												//与上一行中心点对比,左右黑线宽度
	uint16_t tLeft=0,tRight=0,tCross;
	
	for(i=0;i<line_count;i++)																			//扫描所有行
	{
		fRight = fLeft = 0;																					//清除左右边界标志
		eagle_praseLine(img_buffer,line_buffer,line_start+i);				//先解压得到行数据，从靠近摄像头开始		
		if (line_buffer[search_start] == 0xff)											//起点为黑,左右边界在两边
		{
				//搜索左边界
				scan_width = search_start;																			//向左扫描长度
				j = search_start;                          											//向左扫描起点
				for (k = 0; k < scan_width; k++)    
				{
						j--;	
						if (line_buffer[j] == 0) 																		//黑->白,左边界
						{              		
								break;                           
						}                       		
				}
				left = j;	
				
				//搜索右边界
				scan_width = CAMERA_W - 1 - search_start;							//向右扫描长度
				j = search_start;                          						//向右扫描起点
				for (k = 0; k < scan_width; k++)    
				{
						j++;
						if (line_buffer[j] == 0) 															//黑->白,右边界
						{
								break;
						}
				}
				right = j; 

				//if(right-left>2)		//黑线至少3个点
				{
					fLeft = 1;
					fRight = 1;  
				}
		}
		else	//起点为白点,左右边界在同一侧
		{
				//先向左搜索
			width_in_white = (i==0)?150:15;
			scan_width = (width_in_white < search_start) ? width_in_white : search_start; //扫描长度,限制白线上的扫描宽度
			j = search_start;                          										//向左扫描起点
			for (k = 0; k < scan_width; k++)   
			{         
					j--;					
					if (line_buffer[j] == 0xff) 																//白->黑,右边界
					{							
						right = j+1;							
						fRight = 1;																							//右边界标记
						break;                
					}                        
			}
		
			//找到了右边界才继续寻找左边界
			if(fRight)																										
			{
					scan_width = j;						            										//向左扫描宽度
					for (k = 0; k < scan_width; k++)    
					{
							j--;  
							if (line_buffer[j] == 0) 															//黑->白,左边界
							{
									break;
							}
					}
					left = j;fLeft=1;
			}
			
			//向左搜不到，向右搜索
			if (fLeft == 0) 																							
			{
				width_in_white = (i==0)?150:15;
				scan_width = (width_in_white < (CAMERA_W - 1 - search_start)) ? width_in_white : (CAMERA_W - 1 - search_start); //扫描长度
				j = search_start;                          																										//向右扫描起点
				for (k = 0; k < scan_width; k++)    
				{
					j++;
					if (line_buffer[j] == 0xff)														//白->黑,左边界
					{
						left = j-1;
						fLeft = 1;
						break;
					}
				}
				//找到了左边界才继续找右边界
				if(fLeft)
				{
						scan_width = CAMERA_W - 1 - j; //扫描长度																	
						for (k = 0; k < scan_width; k++)   
						{
								j++; 		
								if (line_buffer[j]  == 0) //黑->白，右边界
								{
										break;
								}
						}
						right = j;
				}
			}
		}

		//计算结果
		if (!(fLeft ==1) && (fRight == 1))							//不能找到两条边界,线条丢失,使用上一行的值
		{
				if (i == 0) result[i] = search_start;  		//使用上一帧的结果
				else    result[i] = result[i-1];			 //使用上一行的结果
		}
		else																					//找到了线条
		{
			
			
			
//			if((left<result[i-1]) && (right>result[i-1]))			//左右边界在上一次两边
//			{
//					width_left = result[i-1]-left;
//					width_right = right - result[i-1];
//			}
//			else if(right<=result[i-1])									//左右边界在上一次左边				
//			{
//				width_left = right-left;
//				width_right = 0;
//			}
//			else																			//左右边界在上一次右边	
//			{
//				width_right = right-left;
//				width_left = 0;
//			}

//			//向右转判断??
//			if((width_right>width_left) && (width_right-width_left>20))
//			{
//				tRight++;
//			}
//			else
//			{
//				
//				tRight=0;
//			}
//			//向左转判断
//			if((width_left>width_right) && (width_left-width_right>20))
//			{
//				tLeft++;
//			}
//			else
//			{
//				tLeft=0;
//			}

			result[i] = (right + left) / 2;
			
			
			if(i>0)
			{
				if((result[i] > result[i-1])&&((result[i]-result[i-1])>5))			//向右转
				{
					tRight++;
				}
				else
				{
					if(tRight)		//确定向右转
					{
						//csp_sendRoute(ROUTE_RIGHT,i);
					}
					tRight = 0;
				}
			}
			
//			else if((result[i]<result[i-1])&&(result[i-1]-result[i]>10)	)	//向左转
//			{
//				tLeft++;
//			}
		}
		
		
		search_start = result[i];
	}
	return result[0];
}



#define ROUTE_ERR_MAX	5
void route_filiter(uint8_t *route,uint16_t count)
{
	uint16_t i;
	int16_t err;
	for(i=1;i<count;i++)
	{
		err = (int16_t)(route[i] - route[i-1]);
		if(ABS(err) > ROUTE_ERR_MAX)								//当前点与前一点插值过大
		{
			err = (int16_t)(route[i+1] - route[i-1]);				
			if(ABS(err) > ROUTE_ERR_MAX)				//前一点与后一点差值过大
			{
				route[i] = route[i-1];					
			}
			else
			{
				route[i] = (route[i+1]+route[i-1])/2;		//前一点与后一点平均值进行插值
			}
		}
	}
}

//最小二乘法求直线斜率
float route_getSlope(uint8_t *route,uint16_t line_start,uint16_t n)
{
	double sumx = 0.0f;
	double sumy = 0.0f;
	double sumxy = 0.0f;
	double sumx2=0.0f;
	uint16_t i;
	route += line_start;
	for(i=0;i<n;i++)
	{
		sumx += i;
		sumy += *route;
		sumxy += (*route)*i;
		sumx2 += i*i;
		route++;
	}	
	return (sumx*sumy - n*sumxy)/(sumx*sumx-n*sumx2);
}


//从line_start开始的那个位置求平均值,返回与中点的误差
//!!中值????,除去最大最小值???
int16_t route_getAverageDistance(uint8_t *route,uint16_t line_start,uint16_t n)
{
	uint32_t sum=0;
	uint16_t i;
	for(i=line_start;i<line_start+n;i++)
	{
		sum+= *route++;
	}
	sum /= n;
	return (int16_t)(sum -ROUTE_MIDPOS);
	
}




