#include "table.h"
#include "sys.h"

#include "controller.h"
#include "cmath.h"

table_t mTab;


void table_load()
{
	flash_read(STM32_FLASH_LASTPAGE,(u16*)&mTab,sizeof(mTab)/2+sizeof(mTab)%2);
	
	if(mTab.TableInited != 'Y')
	{
		debug("init table\r\n");
		mTab.threshold = 0xff;
		mTab.fSave = 0;
		mTab.TableInited = 'Y';
		//–¥»Îflash
		flash_write(STM32_FLASH_LASTPAGE,(u16*)&mTab,sizeof(mTab)/2+sizeof(mTab)%2);
	}
}


void table_save()
{
	/*PID*/
	mTab.threshold = meagle.threshold;
	mTab.fSave = 0;
	flash_write(STM32_FLASH_LASTPAGE,(u16*)&mTab,sizeof(mTab)/2+sizeof(mTab)%2);
}
