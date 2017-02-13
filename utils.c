#include "utils.h"

void delay(int c) {
	int i;
	for(i = c; i > 0; i--);
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
byte spi(byte data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}