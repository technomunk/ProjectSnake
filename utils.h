/*
	A helper utility library
*/

#ifndef PS_UTILITY
#define PS_UTILITY

#include <pic32mx.h>
#include <stdint.h>

typedef uint8_t byte;
void delay(int c);

void spi_initialize();
byte spi(byte data);

#endif // !PS_UTILITY