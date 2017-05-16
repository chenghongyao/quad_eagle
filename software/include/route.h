#ifndef _ROUTE_H_
#define _ROUTE_H_

#include "sys.h"
#include "eagle.h"



#define ROUTE_MIDPOS	(CAMERA_W/2)				//ÖĞµãÎ»ÖÃ


uint16_t route_find(uint8_t *img_buffer,uint8_t *result,uint16_t line_start,uint16_t line_count,uint16_t search_start);
int16_t route_getAverageDistance(uint8_t *route,uint16_t line_start,uint16_t n);
float route_getAngle(uint8_t *route,uint16_t line_start,uint16_t n);
#endif

