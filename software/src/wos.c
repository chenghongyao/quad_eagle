#include "wos.h"
#include "sys.h"

#define WOS_THREAD_MAX 5			//��Ҫ���ó���127




static pOSFunc_t pOSFuncCollector[WOS_THREAD_MAX];		//�̺߳�������
static uint16_t  OSCountMax[WOS_THREAD_MAX];			//�������ֵ
static uint8_t	 OSCounterFlag[WOS_THREAD_MAX];			//������ɱ�־
static uint16_t  OSCounter[WOS_THREAD_MAX];				//������
static pOSFunc_t OSmain;


#ifdef WOS_USE_TIMERECORD
static uint32_t OStStart[WOS_THREAD_MAX];				//��ʼʱ��
static uint32_t OSPeriod[WOS_THREAD_MAX];				//��������
static uint32_t OSTimeUse[WOS_THREAD_MAX];				//����ʱ��
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
���ܣ���Ƶ��תΪ�������ֵ
������Ƶ��
����: �������ֵ
ʱ�䣺1/19/2017
ע�⣺ 
*******************************************************************************/  
uint16_t  wos_fre2count(uint16_t fre)
{
	return (uint16_t)(1000 / fre);		//����һ������ʱ��(ms)
}

//////////////////////////////////////////////////////////////////////////
//��ӳɹ�����id,ʧ�ܷ���-1
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
//��ȡ������������
////////////////////////////////////////////////////
uint32_t  wos_period(uint8_t  id)
{
	return OSPeriod[id];
}
/////////////////////////////////////////////////////
//��ȡ����ִ��ʱ��
////////////////////////////////////////////////////
uint32_t wos_time(uint8_t id)
{
	return OSTimeUse[id];
}
#endif

/////////////////////////////////////////////////////
//���к󽫲��ٷ���
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
				OSPeriod[i] = tNow - OStStart[i];		//��ȡ��������
				OStStart[i] = tNow;			
#endif

				pOSFuncCollector[i]();

#ifdef WOS_USE_TIMERECORD
				OSTimeUse[i] = micros() - OStStart[i];	//��ȡ����ִ��ʱ��
#endif

				OSCounterFlag[i] = 0;

			}
		}
		
		if(OSmain!=NULL)
			OSmain();
	}
}

/////////////////////////////////////////////////////
//ÿms����һ��
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
