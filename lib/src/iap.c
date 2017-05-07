#include "iap.h"
#include "sys.h"

typedef void(*iapfun)(void);
iapfun jumpapp2;

void iap_run(uint32_t appaddr)
{
	if (((*(vu32*)appaddr) & 0x2FFE0000) == 0x20000000) //检查栈顶是否合法
	{
		jumpapp2 = (iapfun)*(vu32*)(appaddr + 4);		//获取复位向量		
		//MSR_MSP(*(vu32*)appaddr);									//初始化APP堆栈指针		
		__set_MSP(*(vu32*)appaddr);
		jumpapp2(); 
	}
}
