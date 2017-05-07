#ifndef _ANO_H
#define _ANO_H


#include "sys.h"
#include  "pid.h"




//����Ϊ����Э��������
#define ANO_CHANNELVALUE		0x03




//���·�����Э��!!! 
#define CSP_NOP					0							//������

//1-49 ��������
#define CSP_ARM_IT					5					//����
#define CSP_DISARM_IT				6					//����
#define CSP_LAND_DOWN 			7					//�Զ��½�
#define CSP_HOLD_ALT 				8					//����
#define CSP_STOP_HOLD_ALT 	9					//ȡ������
#define CSP_HEAD_FREE 			10				//��ͷģʽ
#define CSP_STOP_HEAD_FREE	11				//ȡ����ͷģʽ
#define CSP_POS_HOLD 				12				//��ͣ
#define CSP_STOP_POS_HOLD 	13				//ȡ����ͣ

#define CSP_ACC_CALI				14				//���ٶ�У׼
#define CSP_GYRO_CALI				15				//���ٶ�У׼
#define CSP_SENSOR_CALI			16				//������У׼

#define CSP_CHANNEL_VALUE		17				//����ͨ������



//50-69 ������������
#define CSP_ASK_STATE				50				//����ɿ�״̬



//70-99 ��������
#define CSP_SET_ROLLERR 		70				//����ROLLƫ��
#define CSP_SET_PITCHERR	 	71				//����PITCHƫ��

#define CSP_PID1						72
#define CSP_PID2						73
#define CSP_PID3						74
#define CSP_PID4						75
#define CSP_PID5						76
#define CSP_PID6						77
#define CSP_PID7						78
#define CSP_PID8						79


//100-119 IAP(�������)����
#define CSP_RES_SUCC		100
#define CSP_RES_FAIL		101
#define CSP_CODESIZE		102
#define CSP_CODEPACK		103
#define CSP_LOADFINISH	104
#define CSP_PACKCOUNT 	105
#define CSP_STARTIAP 		106
#define CSP_CHECK				108


//120-199,������������
#define CSP_ACCEL_DATA 120



/////////////////////////////////////////////////////
//ң��->�ɿ� 		��Ϊ0
//�ɿ�->��λ�� 	��Ϊ1
////////////////////////////////////////////////////
#define ANO_SENDTO_PC 1		


void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, s32 alt, uint8_t fly_model, uint8_t armed);
void ANO_DT_Send_Sensor(int16_t a_x,int16_t a_y,int16_t a_z,int16_t g_x,int16_t g_y,int16_t g_z,int16_t m_x,int16_t m_y,int16_t m_z);
void ANO_DT_Send_RCData(uint16_t thr,uint16_t yaw,uint16_t rol,uint16_t pit,uint16_t aux1,uint16_t aux2,uint16_t aux3,uint16_t aux4,uint16_t aux5,uint16_t aux6);
void ANO_DT_Send_Power(uint16_t votage, uint16_t current);
void ANO_DT_Send_MotoPWM(uint16_t m_1,uint16_t m_2,uint16_t m_3,uint16_t m_4,uint16_t m_5,uint16_t m_6,uint16_t m_7,uint16_t m_8);
void ANO_DT_Send_PID(uint8_t group,float p1_p,float p1_i,float p1_d,float p2_p,float p2_i,float p2_d,float p3_p,float p3_i,float p3_d);
void ANO_DT_SendPidDetail(uint8_t name,pid_t *pid);
void ANO_SendCustomData(uint8_t name,const char *fmt,...);
void ANO_DT_Send_Control(int16_t THR, int16_t YAW, int16_t ROL, int16_t PIT, int16_t AUX1, int16_t AUX2, int16_t AUX3, int16_t AUX4);
void ANO_RecvBytes(uint8_t *dat,uint16_t len);
void ANO_RecvByte(uint8_t dat);

#endif

