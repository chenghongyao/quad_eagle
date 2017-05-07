#include "mpu6050_dev.h"
#include "mpu6050.h"
#include "sys.h"

#include "cmath.h"





//�ײ������ӿ�===============================================================
static void MPU_WriteByte(u8 reg_addr,u8 reg_dat)
{
	 IIC_DevWriteByte(MPU_SlaveAddress,reg_addr, reg_dat);
}
static void MPU_ReadBuf(u8 reg_addr,u8* pBuf,u8 len)
{
	IIC_DevReadBuf(MPU_SlaveAddress,reg_addr,len,pBuf);
}
//=================================================================



void mpu6050_init()
{
	MPU_WriteByte(PWR_MGMT_1, 0x80);						//��λ
	delay_ms(50);
	
	//�����ǲ���Ƶ������ ������Ƶ��=���������Ƶ��/(1+SMPLRT_DIV),
	//DLPFʹ��ʱ���������Ƶ��Ϊ1k,����Ϊ8k
	//��һ�����ò�������Ƶ
	MPU_WriteByte(SMPLRT_DIV, 0x00);	//���������ǲ���Ƶ��
	MPU_WriteByte(PWR_MGMT_1, 0x03);	//�������״̬,PLLwith gyroz
	MPU_WriteByte(PIN_CONFIG, 0x02);	//IIC�ӻ��ܽ�ֱͨ
	MPU_WriteByte(MPU_CONFIG, MPU6050_DLPF_BW_42);	///DLPF����:ACC=44Hz,GYRO=42hZ(���Ƶ��1k)
	MPU_WriteByte(GYRO_CONFIG, MPU_GYROCFG_FSSEL_2000);	//��������
	MPU_WriteByte(ACCEL_CONFIG,MPU_ACCELCFG_AFSSEL_8G);
	
	delay_ms(100);	//�ȴ�ģ���ȶ�,�����������������ƫ��̫��
}





void mpu6050_getGyroAdc(int16_t *gyroadc)
{
	uint8_t  gyrobuffer[6];
	MPU_ReadBuf(GYRO_XOUT_H,gyrobuffer,6);		
	gyroadc[0]=(int16_t)((gyrobuffer[0]<<8) | gyrobuffer[1]);
	gyroadc[1]=(int16_t)((gyrobuffer[2]<<8) | gyrobuffer[3]);
	gyroadc[2]=(int16_t)((gyrobuffer[4]<<8) | gyrobuffer[5]);
}

void mpu6050_getAccAdc(int16_t *accadc)
{
	uint8_t accbuffer[6];
	MPU_ReadBuf(ACCEL_XOUT_H,accbuffer,6);		
	accadc[0]=(int16_t)((accbuffer[0]<<8) | accbuffer[1]);
	accadc[1]=(int16_t)((accbuffer[2]<<8) | accbuffer[3]);
	accadc[2]=(int16_t)((accbuffer[4]<<8) | accbuffer[5]);
}


void mpu6050_getAccel(float *acc)
{
	uint8_t accbuffer[6];
	MPU_ReadBuf(ACCEL_XOUT_H,accbuffer,6);		
	acc[0]=MPU_K_ACCEL*(int16_t)((accbuffer[0]<<8) | accbuffer[1]);
	acc[1]=MPU_K_ACCEL*(int16_t)((accbuffer[2]<<8) | accbuffer[3]);
	acc[2]=MPU_K_ACCEL*(int16_t)((accbuffer[4]<<8) | accbuffer[5]);
}


/////////////////////////////////////////////////////
//��λ:��/s
////////////////////////////////////////////////////
void mpu6050_getGyro(float *gyro)
{
	uint8_t  gyrobuffer[6];
	MPU_ReadBuf(GYRO_XOUT_H,gyrobuffer,6);		
	gyro[0] = MPU_K_GYRO*(int16_t)((gyrobuffer[0] << 8) | gyrobuffer[1]);
	gyro[1] = MPU_K_GYRO*(int16_t)((gyrobuffer[2] << 8) | gyrobuffer[3]);
	gyro[2] = MPU_K_GYRO*(int16_t)((gyrobuffer[4] << 8) | gyrobuffer[5]);
}
	
