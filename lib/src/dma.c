#include "dma.h"
#include "sys.h"


void dma1_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//单向，外设到内存

	DMA_InitStructure.DMA_PeripheralBaseAddr = 0;														//外设地址
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设地址不变
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//8位

	DMA_InitStructure.DMA_MemoryBaseAddr = 0;			//内存地址
	DMA_InitStructure.DMA_BufferSize = 2;					//内存大小
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//8位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址自增
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 			//连续接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//???

	
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	

	
}
