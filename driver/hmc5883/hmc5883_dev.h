#ifndef _HMC5883_DEV_H
#define _HMC5883_DEV_H
#include "sys.h"

//�Ĵ�����ַ
#define HMC_CONFIGA		0x00	//��������������ʺͲ������� MA���ƽ����,DO�������,MS��������
#define HMC_CONFIGB		0x01	//��������(����)
#define HMC_MODE		0x02
#define HMC_XMSB		0x03
#define HMC_XLSB		0x04
#define HMC_ZMSB		0x05
#define HMC_ZLSB		0x06
#define HMC_YMSB		0x07
#define HMC_YLSB		0x08
#define HMC_STATUS		0x09
#define HMC_IDA			0x0A
#define HMC_IDB			0x0B
#define HMC_IDC			0x0C

//=============================================



/*HMC_CONFIGA*/
//ÿ�β��������ѡ���ƽ����
#define HMC5883_AVRCOUNTS_1	(0<<5)
#define HMC5883_AVRCOUNTS_2	(1<<5)
#define HMC5883_AVRCOUNTS_4	(2<<5)
#define HMC5883_AVRCOUNTS_8 (3<<5)
//�������
#define HMC5883_SPEED_0_75 	(0<<2)
#define HMC5883_SPEED_1_5 	(1<<2)
#define HMC5883_SPEED_3 		(2<<2)
#define HMC5883_SPEED_7_5 	(3<<2)
#define HMC5883_SPEED_15 		(4<<2)
#define HMC5883_SPEED_30 		(5<<2)
#define HMC5883_SPEED_75 		(6<<2)
//��������
#define HMC5883_MS_NORMAL 		0
#define HMC5883_MS_POSTIVE 		1
#define HMC5883_MS_NEGATIVE		2


/*HMC_CONFIGB*/
#define HMC5883_GN_1370	0x00	//0.88Ga -�Ƽ���������Χ
#define HMC5883_GN_1090	0x01	//1.3Ga
#define HMC5883_GN_820	0x02	//1.9Ga
#define HMC5883_GN_660	0x03	//2.5Ga
#define HMC5883_GN_440	0x04	//4.0Ga
#define HMC5883_GN_390	0x05	//4.7Ga
#define HMC5883_GN_330	0x06	//5.6Ga
#define HMC5883_GN_230	0x07	//8.1Ga

/*HMC_MODE*/
#define HMC5883_MR_COUNTINUS	0x00		//��������
#define HMC5883_MR_ONE				0x01		//���β���
#define HMC5883_MR_IDLE				0x00		//����ģʽ

/*HMC STATUS*/
#define HMC5883_SR_LOCK				0x02		//
#define HMC5883_SR_RDY				0x01		//






#define  HMC_IIC_ADDRESS 0x3C



#endif
