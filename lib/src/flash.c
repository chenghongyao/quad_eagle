#include "flash.h"
#include "sys.h"



/////////////////////////////////////////////////////
//д֮ǰ������������
//len���ܳ��������� ��ַΪ0x400��0x800��������
////////////////////////////////////////////////////
void flash_write(uint32_t address, void *buf,uint16_t len)
{
	__IO uint16_t *pbuf = (uint16_t *)buf;
	uint16_t i;

	FLASH_Unlock();
	FLASH_ErasePage(address);
	for (i = 0; i < len/2 ; i++)
	{
		FLASH_ProgramHalfWord(address,*pbuf++);
		address+=2;
	}
	FLASH_Lock();
}

void flash_read(uint32_t address, void *buf,uint16_t len)
{
	__IO uint16_t *pbuf = (uint16_t *)buf;
	uint16_t i;

	for (i = 0; i < len/2 ; i++)
	{
		*pbuf++ = *(vu16*)address;
		address += 2;
	}
}




