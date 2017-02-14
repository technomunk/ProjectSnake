/*
	A helper utility library
*/

#ifndef PS_UTILITY
#define PS_UTILITY

#include <pic32mx.h>
#include <stdint.h>

typedef uint8_t byte;

void spi_initialize();
byte spi(byte data);
byte spi_array(byte * pData, unsigned int dataSize);

#endif // !PS_UTILITY