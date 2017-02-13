/*
	A class of high level OLED display based functions
	
	all methods have display_ prefix, similar to Display:: syntax in higher level languages
*/

#include "display.h"

byte display_buffer[PSD_COLUMN_COUNT][PSD_PAGE_COUNT];

/*	
	Initializes everything necessary for the display to work
	Should be called before any other functios are
	=========================================================
	by
		Grigory Glukhov
*/
void display_initialize() {
	
	// Set up pins
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	// Reset voltage and data
	PSD_PORT_COMMAND &= ~PSD_MASK_COMMAND;
	PSD_PORT_VDD &= ~PSD_MASK_VDD;
	
	// display off
	spi(0xAE);
	
	// Reset the display
	PSD_PORT_RESET &= ~PSD_MASK_RESET;
	delay(10);
	PSD_PORT_RESET |= PSD_MASK_RESET;
	
	// Set charge pump
	spi(0x8D);
	spi(0x14);
	
	// Set Pre-charge period
	spi(0xD9);
	spi(0xF1);
	
	// Apply voltage
	PSD_PORT_VBATT &= PSD_MASK_VBATT;
	delay(10000000);	// hope for 100 ms
	
	// Remap the display so that the origin is in left top corner
	spi(0xA1);	// remap columns
	spi(0xC8);	// remap rows
	
	// COM related stuff
	spi(0xDA);	// COM config
	spi(0x20);	// sequential COM
	
	// Display on command
	spi(0xAF);
}

/*
	Terminates the display in a proper manner
	Don't think it's actually used, but thats what documentation calls for
	======================================================================
	by
		Grigory Glukhov
*/
void display_terminate() {
	// display off
	spi(0xAE);
	
	PSD_PORT_VBATT &= ~PSD_MASK_VBATT;
	delay(10000000);	// hope for 100 ms
	PSD_PORT_VDD &= PSD_MASK_VDD
}

/*
	Clears the buffer and the display at the same time
	Should be called right after initialization, but can be used elsewhere
	======================================================================
	by
		Grigory Glukhov
*/
void display_clear() {
	
	int page, x;
	
	for (page = 0; page < PSD_PAGE_COUNT; page++) {
		PSD_PORT_COMMAND &= ~PSD_MASK_COMMAND;
		
		spi(0x22);
		spi(page);
		
		spi(0xF);
		spi(0x10);
		
		PSD_PORT_COMMAND |= PSD_MASK_COMMAND;
		
		for (x = 0; x < PSD_COLUMN_COUNT; x++) {
			display_buffer[x][page] = 0x00;
			spi(display_buffer[x][page]);
		}
	}
}

/*
	Updates part of display buffer
	Does NOT change the displayed image
	call display_show() for that
	===================================
	by
		Grigory Glukhov
*/
void display_update(int x, int y, byte flag) {
	
	int page = y / 8;
	y %= 8;
	flag &= 1;
	
	if (flag)
		display_buffer[x][page] |= (1 << y);
	else
		display_buffer[x][page] &= ~(1 << y);
}

/*
	Displays the buffered image to the display
	Use this after changes
	Will look weird and broken if used too often
	============================================
	by
		Grigory Glukhov
*/
void display_show() {
	int page, x;
	
	for (page = 0; page < PSD_PAGE_COUNT; page++) {
		PSD_PORT_COMMAND &= ~PSD_MASK_COMMAND;
		
		spi(0x22);
		spi(page);
		
		spi(0xF);
		spi(0x10);
		
		PSD_PORT_COMMAND |= PSD_MASK_COMMAND;
		
		for (x = 0; x < PSD_COLUMN_COUNT; x++) {
			spi(display_buffer[x][page]);
		}
	}
}