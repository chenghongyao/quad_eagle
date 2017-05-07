#include "sys.h"
#include "bytes.h"






uint32_t  BytesToUINT32(uint8_t *bys)
{
	return (bys[0] | bys[1] << 8 | bys[2] << 16 | bys[3] << 24);
}


uint16_t BytesToUINT16(uint8_t *bys)
{
	return (bys[0]|bys[1]<<8);
}
int16_t BytesToINT16(uint8_t *bys)
{
	return (bys[0]|bys[1]<<8);
}




uint16_t  BigBytesToUINT16(uint8_t *bys)
{
	return (bys[1] | bys[0] << 8);
}
int16_t BigBytesToINT16(uint8_t *bys)
{
	return (bys[1] | bys[0] << 8);
}



float BytesToFloat(uint8_t *bys)
{
	return (bys[0] | bys[1] << 8 | bys[2] << 16 | bys[3] << 24);
}
