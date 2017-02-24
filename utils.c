#include "utils.h"

char intStr[] = "4294967295";	// intStr should be max possible size, so I set it to max value from intToStr();

long holdrand;

char * intToStr(unsigned int i, int * pSize) {
	int size = 0;
	do {
		intStr[sizeof(intStr) - ++size] = '0' + (i % 10);
		i /= 10;
	} while (i);
	
	if (pSize)
		*pSize = size;
	
	return &intStr[sizeof(intStr) - size];
}

/*
	Initializes all required ports for SPI protocol
	===============================================
	by
		Grigory Glukhov
*/
void spi_initialize() {
	// Set up SPI as master
	SPI2CON = 0;
	SPI2BRG = 4;

	// Clear SPIROV
	SPI2STATCLR &= ~0x40;
	// Set CKP = 1, MSTEN = 1;
	SPI2CON |= 0x60;

	// Turn on SPI
	SPI2CONSET = 0x8000;
}

/*
	Sends and recieves a byte over SPI
	==================================
	by
		Grigory Glukhov
*/
byte spi_byte(byte data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}

/*
	Sends a whole array of bytes over SPI
	=====================================
	by
		Grigory Glukhov
*/
byte spi_array(byte * pData, unsigned int dataSize) {
	byte tmp;
	while (dataSize--) {
		while(!(SPI2STAT & 0x08));
		SPI2BUF = *pData++;
		while(!(SPI2STAT & 0x01));
		tmp = SPI2BUF;
	}
	return tmp;
}

void rand_seed(unsigned int seed) {
	holdrand = (long)seed;
}

int rand_next() {
	return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}