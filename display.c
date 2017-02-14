/*
	A class of high level OLED display based functions
	
	all methods have display_ prefix, similar to Display:: syntax in higher level languages
*/

#include "display.h"

byte display_buffer[PSD_SIZE_BUFFER];

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
	time_wait(1);
	PSD_PORT_RESET |= PSD_MASK_RESET;
	
	// Set charge pump
	spi(0x8D);
	spi(0x14);
	
	// Set Pre-charge period
	spi(0xD9);
	spi(0xF1);
	
	// Apply voltage
	PSD_PORT_VBATT &= PSD_MASK_VBATT;
	time_wait(100);
	
	// Remap the display so that the origin is in left top corner
	spi(0xA1);	// remap columns
	spi(0xC8);	// remap rows
	
	// COM related stuff
	spi(0xDA);	// COM config
	spi(0x20);	// sequential COM
	
	spi(0x20);
	spi(0x00);
	
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
	time_wait(100);
	PSD_PORT_VDD &= PSD_MASK_VDD;
}

/*
	Clears the buffer and the display at the same time
	Should be called right after initialization, but can be used elsewhere
	======================================================================
	by
		Grigory Glukhov
*/
void display_clear() {
	
	int id;
	
	for (id = 0; id < PSD_SIZE_BUFFER; id++)
		display_buffer[id] = 0x00;
}

/*
	Updates part of display buffer
	Does NOT change the displayed image
	call display_show() for that
	===================================
	by
		Grigory Glukhov
*/
void display_put(int x, int y, byte flag) {
	
	int page = (y / 8) * PSD_COLUMN_COUNT;
	y %= 8;
	
	if (flag)
		display_buffer[page + x] |= (1 << y);
	else
		display_buffer[page + x] &= ~(1 << y);
}

/*
	Updates a single pixel immediatly
	=================================
	by
		Grigory Glukhov
*/
void display_update(int x, int y, byte flag) {
	
	PSD_PORT_COMMAND &= ~PSD_MASK_COMMAND;
	
	int page = y / 8;
	
	// Send page number here
	spi(0xB0 | page);
	
	// Send row number
	spi(x & 0xF);					// low nibble
	spi(0x10 | ((x >> 4) & 0xF));	// high nibble
	
	
	page *= PSD_COLUMN_COUNT;
	y %= 8;
	
	if (flag)
		display_buffer[page + x] |= (1 << y);
	else
		display_buffer[page + x] &= ~(1 << y);
	
	
	PSD_PORT_COMMAND |= PSD_MASK_COMMAND;
	spi(display_buffer[page + x]);
}

/*
	Sends a single change brightness command to the display
	=======================================================
	by
		Grigory Glukhov
*/
void display_setBrightness(byte b) {
	PSD_PORT_COMMAND &= ~PSD_MASK_COMMAND;
		
	spi(0x81);
	spi(b);
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
	
	int id;
	
	PSD_PORT_COMMAND &= ~PSD_MASK_COMMAND;
	
	// Reset page
	spi(0x22);	// send set row command
	spi(0);		// starting page
	spi(3);		// ending page
	
	
	// Reset column
	spi(0x21);	// send set column command
	spi(0);		// starting column
	spi(127);	// ending column

	PSD_PORT_COMMAND |= PSD_MASK_COMMAND;
	
	for (id = 0; id < PSD_SIZE_BUFFER; id++)
		spi(display_buffer[id]);
		
}