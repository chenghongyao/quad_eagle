#ifndef _SCCB_H_
#define _SCCB_H_
#include "sys.h"



void SCCB_Init(void);
uint8_t SCCB_WriteReg(uint8_t dev_addr,uint8_t reg_addr,uint8_t reg_val);
uint8_t SCCB_ReadReg(uint8_t dev_addr,uint8_t reg_addr,uint8_t *val);	//ÏÈ¶ÁµÍ×Ö½Ú

#endif

