#ifndef _TABLE_H
#define _TABLE_H
#include "sys.h"

typedef struct
{
	uint8_t TableInited;
	
	uint8_t threshold;
	
	uint8_t fSave;
	/**/
}table_t;

extern table_t mTab;
void table_load(void);
void table_save(void);

#endif
