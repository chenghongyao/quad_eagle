#ifndef _ALT_H
#define _ALT_H

#include "sys.h"



//typedef struct
//{
//	float alt_target;
//	float rate_target;
//}alt_t;
//extern alt_t mAlt;

void alt_init(void);
void alt_update_ekf(void);
void alt_update_ultra(void);



#endif

