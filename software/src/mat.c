#include "mat.h"
#include "sys.h"

#include "bytes.h"


static uint8_t mat_txbuf[50] = { 0xAA, 0xAA };	//发送数据缓存
//static uint8_t mat_rxbuffer[50] = { 0xAA, 0xAA };	//接收数据缓存
static uint8_t mat_txindex = 4;



// static void mat_send_data(uint8_t *dataToSend, uint8_t length)
// {
// 	//	tcp_send(0,dataToSend,length);
// 	myputbuf(mat_txbuf, length);
// 	//nrf24l01_sendPacket(dataToSend,length);
// 	//nrf24l01_sendAckPacket(dataToSend,length);
// 
// }




//static void mat_put8(uint8_t dat)
//{
//	mat_txbuf[mat_txindex++] = dat;
//}
//static void mat_put16(uint16_t  dat)
//{
//	mat_txbuf[mat_txindex++] = dat;
//	mat_txbuf[mat_txindex++] = dat >> 8;
//}
//static void mat_put32(uint32_t  dat)
//{
//	mat_txbuf[mat_txindex++] = BYTE0(dat);
//	mat_txbuf[mat_txindex++] = BYTE1(dat);
//	mat_txbuf[mat_txindex++] = BYTE2(dat);
//	mat_txbuf[mat_txindex++] = BYTE3(dat);
//}
static void mat_putf32(float dat)
{
	mat_txbuf[mat_txindex++] = BYTE0(dat);
	mat_txbuf[mat_txindex++] = BYTE1(dat);
	mat_txbuf[mat_txindex++] = BYTE2(dat);
	mat_txbuf[mat_txindex++] = BYTE3(dat);
}

static void mat_setName(u8 name)
{
	mat_txbuf[2] = name;
	mat_txindex = 4;
}



//数据上传
static void mat_upload(void)
{
	uint8_t i;
	uint8_t check = 0;
	mat_txbuf[3] = (mat_txindex - 4);
	for (i = 0; i < mat_txindex; i++)
	{
		check += mat_txbuf[i];
	}
	mat_txbuf[mat_txindex++] = check;
	MAT_SENDDATA(mat_txbuf, mat_txindex);
}




void mat_sendAccel(float ax, float ay, float az)
{
	mat_setName(MAT_DS_ACCEL);
	mat_putf32(ax);
	mat_putf32(ay); 
	mat_putf32(az);
	mat_upload();
}

void mat_sendGyro(float gx, float gy, float gz)
{
	mat_setName(MAT_DS_GYRO);
	mat_putf32(gx);
	mat_putf32(gy);
	mat_putf32(gz);
	mat_upload();
}


void mat_sendStatus(float roll, float pitch, float yaw)
{
	mat_setName(MAT_DS_STATUS);
	mat_putf32(roll);
	mat_putf32(pitch);
	mat_putf32(yaw);
	mat_upload();
}

//用于椭球拟合
void mat_sendEllipsoid(float x,float y,float z)
{
	mat_setName(MAT_DS_ELLIPSOID);
	mat_putf32(x);
	mat_putf32(y);
	mat_putf32(z);
	mat_upload();
}

//发送滤波数据
void mat_sendLPF(float raw,float lpf)
{
	mat_setName(MAT_DS_LPF);
	mat_putf32(raw);
	mat_putf32(lpf);
	mat_upload();
}

//用于噪声拟合
void mat_sendNoise(float dat)
{
	mat_setName(MAT_DS_NOISE);
	mat_putf32(dat);
	mat_upload();
}


//az = az_b - gz;
//az = k*dz;
//k=4p/m=>p=km/4;
void mat_sendThrustFit(float az,float dz,float gz)
{
	mat_setName(MAT_DS_THROFIT);
	mat_putf32(az);
	mat_putf32(dz);
	mat_upload();
	
}



