#include "wos.h"
#include "sys.h"

#define WOS_THREAD_MAX 5			//不要设置超过127




static pOSFunc_t pOSFuncCollector[WOS_THREAD_MAX];		//线程函数数组
static uint16_t  OSCountMax[WOS_THREAD_MAX];			//计数最大值
static uint8_t	 OSCounterFlag[WOS_THREAD_MAX];			//计数完成标志
static uint16_t  OSCounter[WOS_THREAD_MAX];				//计数器
static pOSFunc_t OSmain;


#ifdef WOS_USE_TIMERECORD
static uint32_t OStStart[WOS_THREAD_MAX];				//起始时间
static uint32_t OSPeriod[WOS_THREAD_MAX];				//调用周期
static uint32_t OSTimeUse[WOS_THREAD_MAX];				//运行时间
#endif


void wos_init(pOSFunc_t mainfun)
{
	uint8_t  i;
	for (i = 0; i < WOS_THREAD_MAX; i++)
	{
		pOSFuncCollector[i] = NULL;
		OSCounterFlag[i] = 0;
	}
	OSmain = mainfun;
}



/******************************************************************************* 
功能：将频率转为所需计数值
参数：频率
返回: 所需计数值
时间：1/19/2017
注意： 
*******************************************************************************/  
uint16_t  wos_fre2count(uint16_t fre)
{
	return (uint16_t)(1000 / fre);		//调用一次所需时间(ms)
}

//////////////////////////////////////////////////////////////////////////
//添加成功返回id,失败返回-1
//////////////////////////////////////////////////////////////////////////
int8_t  wos_add(pOSFunc_t pfunc,uint16_t frequency)
{
	uint8_t  i;
	for (i = 0; i < WOS_THREAD_MAX; i++)
	{
		if (pOSFuncCollector[i]==NULL)
		{		
			OSCountMax[i] = wos_fre2count(frequency);
			OSCounter[i] = 0;
			pOSFuncCollector[i] = pfunc;
			//printf("max=%d\r\n",OSCountMax[i]);
			return i;
		}
	}
	return -1;
}


void wos_remove(uint8_t id)
{
	if (id<WOS_THREAD_MAX)
	{
		pOSFuncCollector[id] = NULL;
		OSCounterFlag[id] = 0;
	}
}

#ifdef WOS_USE_TIMERECORD
/////////////////////////////////////////////////////
//获取函数调用周期
////////////////////////////////////////////////////
uint32_t  wos_period(uint8_t  id)
{
	return OSPeriod[id];
}
/////////////////////////////////////////////////////
//获取函数执行时间
////////////////////////////////////////////////////
uint32_t wos_time(uint8_t id)
{
	return OSTimeUse[id];
}
#endif

/////////////////////////////////////////////////////
//运行后将不再返回
////////////////////////////////////////////////////
void wos_run()
{
	uint8_t  i;
	while (1)
	{
		for (i = 0; i < WOS_THREAD_MAX; i++)
		{
			if (OSCounterFlag[i])
			{
#ifdef WOS_USE_TIMERECORD

				uint32_t tNow = micros();
				OSPeriod[i] = tNow - OStStart[i];		//获取调用周期
				OStStart[i] = tNow;			
#endif

				pOSFuncCollector[i]();

#ifdef WOS_USE_TIMERECORD
				OSTimeUse[i] = micros() - OStStart[i];	//获取函数执行时间
#endif

				OSCounterFlag[i] = 0;

			}
		}
		
		if(OSmain!=NULL)
			OSmain();
	}
}

/////////////////////////////////////////////////////
//每ms调用一次
////////////////////////////////////////////////////
void wos_update()
{
	uint8_t  i;
	for (i = 0; i < WOS_THREAD_MAX; i++)
	{
		if ((pOSFuncCollector[i] != NULL) && (++OSCounter[i] >= OSCountMax[i]))
		{
			OSCounterFlag[i] = 1;
			OSCounter[i] = 0;
		}
	}
}
