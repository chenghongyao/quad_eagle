#include "dma.h"
#include "sys.h"


void dma1_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//�������赽�ڴ�

	DMA_InitStructure.DMA_PeripheralBaseAddr = 0;														//�����ַ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//�����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//8λ

	DMA_InitStructure.DMA_MemoryBaseAddr = 0;			//�ڴ��ַ
	DMA_InitStructure.DMA_BufferSize = 2;					//�ڴ��С
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//8λ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//�ڴ��ַ����
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 			//��������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;		//�����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//???

	
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	

	
}
