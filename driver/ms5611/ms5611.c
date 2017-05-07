#include "ms5611.h"
#include "ms5611_dev.h"
#include "sys.h"
#include "filter.h"

#include "math.h"
#include "ano.h"


ms5611_t mMS5611;


static lpf2_t lpf_baro;
static float win_baro[10];
static uint16_t index_baro=0;

static lpf2_t lpf_press;



static uint32_t  ms5611_delayTime,ms5611_startTime; //延时时间记录
static int32_t  tempCache;													//温度缓存
static uint16_t  PROM_C[7]; //标定值存放


// 延时表单位 us不同的采样精度对应不同的延时值
static uint32_t  MS5611_Delay_us[9] = {
	1500,	//MS561101BA_OSR_256 0.9ms  0x00
	1500,	//MS561101BA_OSR_256 0.9ms  
	2000,	//MS561101BA_OSR_512 1.2ms  0x02
	2000,	//MS561101BA_OSR_512 1.2ms
	3000,	//MS561101BA_OSR_1024 2.3ms 0x04
	3000,	//MS561101BA_OSR_1024 2.3ms
	5000,	//MS561101BA_OSR_2048 4.6ms 0x06
	5000,	//MS561101BA_OSR_2048 4.6ms
	11000,//MS561101BA_OSR_4096 9.1ms 0x08
};


void ms5611_reset()
{
	IIC_Start();
	IIC_WriteByte(MS5611BA_ADDR);
	IIC_WaitAck();
	IIC_WriteByte(MS561101BA_RESET);
	IIC_WaitAck();
	IIC_Stop();
}


void ms5611_readPROM(void)
{
	uint8_t  i;
	uint16_t  inth,intl;
	for (i=0;i<7;i++) 
	{
		IIC_Start();
		IIC_WriteByte(MS5611BA_ADDR);
		IIC_WaitAck();
		IIC_WriteByte(MS561101BA_PROM_BASE_ADDR + i*2);
		IIC_WaitAck();	
		IIC_Stop();
		delay_us(5);
		
		IIC_Start();
		IIC_WriteByte(MS5611BA_ADDR+1);  //进入接收模式	
		delay_us(1);
		IIC_WaitAck();
		inth = IIC_ReadByte(1);  //带ACK的读数据
		delay_us(1);
		intl = IIC_ReadByte(0);	 //最后一个字节NACK
		IIC_Stop();		
		PROM_C[i] = ((inth << 8) | intl);
	}
}

void ms5611_startConversion(uint8_t cmd) 
{
	IIC_Start();
	IIC_WriteByte(MS5611BA_ADDR);
	IIC_WaitAck();
	IIC_WriteByte(cmd);
	IIC_WaitAck();
	IIC_Stop();
}


uint32_t  ms5611_getConversion() 
{
	uint8_t  temp[3];
	uint32_t  result=0;
	
	IIC_Start();
	IIC_WriteByte(MS5611BA_ADDR);
	IIC_WaitAck();
	IIC_WriteByte(0);		//读
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_WriteByte(MS5611BA_ADDR+1);
	IIC_WaitAck();
	temp[0] = IIC_ReadByte(1);IIC_Ack();
	temp[1] = IIC_ReadByte(1);IIC_Ack();
	temp[2] = IIC_ReadByte(0);IIC_NAck();
	IIC_Stop();
	result = ((uint32_t)temp[0]<<16) + ((uint32_t)temp[1]<<8) + temp[2];
	return result;
}


uint8_t ms5611_caliThread(void)
{
	static double paOffsetSum = 0.0f;
	static uint16_t cnt = 0;
	static uint8_t step = 0;
	if(step == 0)	//进行预读取
	{
		if(++cnt == MS5611_PREINIT_NUM)
		{
			cnt = 0;
			step = 1;
		}
	}
	else
	{
		paOffsetSum += mMS5611.Pressure;		
		if(++cnt == MS5611_INIT_NUM)
		{
			mMS5611.PressureZero = paOffsetSum/cnt;
			cnt = 0;
			paOffsetSum = 0.0f;
			mMS5611.PressInited = 1;				
			step = 0;
			return 1;
		}
	}
	return 0;
}



float ms5611_getAltitude(void)
{
	float rawAltitude;		//AltPre;
	rawAltitude = 4433000.0f *0.01f *(1 - pow((mMS5611.Pressure/ mMS5611.PressureZero), 0.1903));
	return rawAltitude;  
}



float GetMedianNum(float *bArray,float xn,uint16_t *index,uint16_t iFilterLen)  
{  
    uint16_t i,j;
    float bTemp;   
		bArray[*index] = xn;
    for (j = 0; j < iFilterLen - 1; j ++)  
    {  
        for (i = 0; i < iFilterLen - j - 1; i ++)  
        {  
            if (bArray[i] > bArray[i + 1])  
            {  
                bTemp = bArray[i];  
                bArray[i] = bArray[i + 1];  
                bArray[i + 1] = bTemp;  
            }  
        }  
    }  
    if ((iFilterLen & 1) > 0)  
    {   
        bTemp = bArray[(iFilterLen + 1) / 2];  
    }  
    else  
    {  

        bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;  
    }  
		
		*index = *index +1;
		if(*index == iFilterLen)
		{
			*index = 0;
		}
    return bTemp;  
}  


void ms5611_getPressure()
{
	int64_t off,sens;
	int64_t TEMP,T2,Aux_64,OFF2,SENS2;  // 64 bits

	int32_t rawPress = ms5611_getConversion();			//获取气压值

	int64_t dT  = tempCache - ((int64_t)PROM_C[5] << 8);	
	TEMP = 2000 + (dT * (int64_t)PROM_C[6])/8388608;

	off  = (((int64_t)PROM_C[2]) << 16) + ((((int64_t)PROM_C[4]) * dT) >> 7);
	sens = (((int64_t)PROM_C[1]) << 15) + (((int64_t)(PROM_C[3]) * dT) >> 8);
	
	if (TEMP < 2000)				//温度低于20度
	{  
		T2 = (dT*dT) >> 31;
		Aux_64 = (TEMP-2000)*(TEMP-2000);
		OFF2 = (5*Aux_64)>>1;
		SENS2 = (5*Aux_64)>>2;

		TEMP = TEMP - T2;
		off = off - OFF2;
		sens = sens - SENS2;
	}
	
	mMS5611.Pressure = (float)(((((int64_t)rawPress) * sens) >> 21) - off) / 32768.0f;
	mMS5611.Temperature =	TEMP*0.01f;	
	if(mMS5611.PressInited == 0)
	{
		ms5611_caliThread();
	}
	else 
	{
		
		mMS5611.alt_raw = ms5611_getAltitude();
		mMS5611.Altitude = lpf2nd_next(&lpf_baro,mMS5611.alt_raw);	
		ANO_DT_Send_Sensor(mMS5611.alt_raw*100,mMS5611.Altitude*100,0,mMS5611.Temperature,0,0,0,0,0);	
		mMS5611.IsBaroUpdated = 1;
//	PINTEST = !PINTEST;
	}
}

//更新周期越20Hz
void ms5611_updateThread()
{
	static uint8_t  step = 0;
	switch(step)
	{
		case 0://启动温度转换
			ms5611_startConversion(MS561101BA_D2+MS5611Temp_OSR);
			ms5611_delayTime = MS5611_Delay_us[MS5611Temp_OSR] ;//转换时间
			ms5611_startTime = micros();		//开始计时
			step++;
		break;
		case 1://温度转换中。。。。。
			if((micros()-ms5611_startTime)>ms5611_delayTime)
			{
				tempCache = (int32_t )ms5611_getConversion();			
				//启动气压转换
				ms5611_startConversion(MS561101BA_D1+MS5611Press_OSR);
				ms5611_delayTime = MS5611_Delay_us[MS5611Press_OSR] ;//转换时间
				ms5611_startTime = micros();		//开始计时
				step++;
			}
		case 2://气压转换中。。。。。
			if((micros()-ms5611_startTime)>ms5611_delayTime)
			{
				ms5611_getPressure();	
				step=0;
			}
		break;
		default :
			step = 0;			
	}
}


void ms5611_init()
{
	mMS5611.PressInited = 0;
	mMS5611.IsBaroUpdated = 0;
	lpf_butter(&lpf_press,20,4);
	lpf_butter(&lpf_baro,20,5);
	
	ms5611_reset();
	delay_ms(100);
	ms5611_readPROM();
	delay_ms(100);
}

