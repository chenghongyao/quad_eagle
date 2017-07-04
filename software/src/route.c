#include "route.h"
#include "sys.h"




#define SCANWIDTH_IN_WHITE		5					//�����ڰ����ϵ����������Χ
//img_buffer:ԭʼͼ������(δ��ѹ)
//result:�������,0Ϊ�������ͷ��һ��
//linie_start:������ʼ��,����,�����ݼ�
//line_count:��ѹline_count��,result��Ҫ���㹻�Ĵ�С
//search_start:��һ��(line_start)���������(ʹ����һ֡�Ľ��)
//����line_start���������
// line_count<=line_start+1

/***
������
���Ϊ�ڵ�ʱ,������ɨ��
1.���Ϊ�׵�ʱ,������ɨ��,���ݺڵ�ľ���Ϳ��ѡ��ȷ��������һ��
2.�����ֶ���ʱ,�������һ�����б�����ɲ�ֵ
3.�����ߺڵ���Ŀ������ʱ˵�����Ǹ�ʮ�ֿ�,��ֻ��һ�ߺܶ�ʱ˵������һ��ֱ�ǹ���
4.���߲�Ӧ��������,ʮ����Ӧ���Ա仯��Ϊ��׼,ͻȻ������ж�Ϊʮ��
**/
uint16_t route_find(uint8_t *img_buffer,uint8_t *result,uint16_t line_start,uint16_t line_count,uint16_t search_start)
{
	uint16_t i,j,k;								
	uint8_t line_buffer[CAMERA_W];		//�н�ѹ����
	uint16_t scan_width;              //ɨ����         
	uint16_t right, left;              //���ұ߽�����
	uint16_t cnt = 0;			//������������
	
	uint16_t width_in_black = 50;
	uint16_t width_in_white = 70;
	
	for(i=0;i<line_count;i++)			
	{
		uint8_t fRight=0,fLeft= 0;				 //�ҵ����ұ߽���		
		eagle_praseLine(img_buffer,line_buffer,line_start-i);				//�Ƚ�ѹ�õ������ݣ��ӿ�������ͷ(������)	
		if (line_buffer[search_start] == 0xff)											//���Ϊ��,���ұ߽�������
		{
				//������߽�
				scan_width = (width_in_black < search_start) ? width_in_black : search_start; //ɨ�賤��
				j = search_start - 1;                          									//����ɨ�����
				for (k = 0; k < scan_width; k++)    
				{
						if (line_buffer[j] == 0) 																		//��->��,��߽�
						{              
								left = j;
								fLeft = 1;
								break;                           
						}                       
						j--;				
				}
				
				if(fLeft)																											//����������߽�ż��������ұ߽�
				{
						scan_width = (width_in_black < (CAMERA_W - 1 - search_start)) ? width_in_black : (CAMERA_W - 1 - search_start); //ɨ�賤��
						j = search_start + 1;                          						//����ɨ�����
						for (k = 0; k < scan_width; k++)    
						{
								if (line_buffer[j] == 0) 															//��->��,�ұ߽�
								{
										right = j;
										fRight = 1;
										break;
								}
								j++;
						}
				}                   
		}
		else																																//���Ϊ�׵�,���ұ߽���ͬһ��
		{
				//����������
				scan_width = (width_in_white < search_start) ? width_in_white : search_start; //ɨ�賤��
				j = search_start - 1;                          									//����ɨ�����
				for (k = 0; k < scan_width; k++)   
				{                    
						if (line_buffer[j] == 0xff) 																//��->��,�ұ߽�
						{
								right = j+1;
								fRight = 1;
								break;                
						}                        
						j--;
				}
				
				if(fRight)																										//�ҵ����ұ߽�ż���Ѱ����߽�
				{
						scan_width = (width_in_black < j) ? width_in_black : j; //ɨ�賤��
						j--;                           														//����ɨ�����
						for (k = 0; k < scan_width; k++)    
						{
								if (line_buffer[j] == 0) 															//��->��,��߽�
								{
										left = j;
										fLeft = 1;
										break;
								}
								j--;
						}
				}
				
				if (fLeft == 0) 																							//�����Ѳ�������������
				{
					scan_width = (width_in_white < (CAMERA_W - 1 - search_start)) ? width_in_white : (CAMERA_W - 1 - search_start); //ɨ�賤��
					j = search_start + 1;                          																										//����ɨ�����
					for (k = 0; k < scan_width; k++)    
					{
							if (line_buffer[j] == 0xff)														//��->��,��߽�
							{
								left = j-1;
								fLeft = 1;
								break;
							}
							j++;
					}
					
					if(fLeft)//�ҵ�����߽�ż������ұ߽�
					{
							scan_width = (width_in_black < (CAMERA_W - 1 - j)) ? width_in_black : (CAMERA_W - 1 - j); //ɨ�賤��
							j++; 																											//����ɨ�����
							for (k = 0; k < scan_width; k++)   
							{
									if (line_buffer[j]  == 0) //��->�ף��ұ߽�
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
		
	
		//������
		if (!(fLeft ==1) && (fRight == 1))//�����ҵ������߽�,ʹ����һ�е�ֵ
		{
				if (i == 0) result[cnt] = search_start;  		//ʹ����һ֡�Ľ��
				else    result[cnt] = result[cnt-1];			 //ʹ����һ�еĽ��
		}
		else
		{
				result[cnt] = (right + left) / 2;
				width_in_black = (right-left)/2+4;		//���ݵ�ǰ���������һ�к��߿��
		}
		search_start = result[cnt];
		width_in_white = SCANWIDTH_IN_WHITE;		//ʣ�µ���ɨ���Ƚ�С 
		cnt++;
	}
	
	return result[0];
}



//��С���˷���ֱ��б��
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


//��line_start��ʼ���Ǹ�λ����ƽ��ֵ,�������е�����
//!!��ֵ????,��ȥ�����Сֵ???
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


////route:ԭʼ·��
////route_res:��Ͻ��
////line_start:�����ʼ��
////n:��ϵ���(��ϴ��ڴ�С)
//void route_fit(uint8_t *route,uint8_t *route_res,uint16_t line_start,uint16_t n)
//{
//	uint16_t i;
//	route_getSlope(route,line_start,n);
//	for(i=0;i<n;i++)
//	{
//		route_res
//	}
//}


