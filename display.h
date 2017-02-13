/*
	A class of high level OLED display based functions
	
	all methods have display_ prefix, similar to Display:: syntax in higher level languages
*/

#ifndef PS_DISPLAY
#define PS_DISPLAY

#include "utils.h"


#define PSD_VDD PORTFbits.RF6
#define PSD_VBATT PORTFbits.RF5
#define PSD_COMMAND PORTFbits.RF4
#define PSD_RESET PORTGbits.RG9

#define PSD_PORT_VDD PORTF
#define PSD_MASK_VDD 0x40

#define PSD_PORT_VBATT PORTF
#define PSD_MASK_VBATT 0x20

#define PSD_PORT_COMMAND PORTF
#define PSD_MASK_COMMAND 0x10

#define PSD_PORT_RESET PORTG
#define PSD_MASK_RESET 0x200

#define PSD_PAGE_COUNT 4
#define PSD_COLUMN_COUNT 128

//	Buffer of display data
//	Avoid adressing directly
extern byte display_buffer[PSD_COLUMN_COUNT][PSD_PAGE_COUNT];

// Initializes everything necessary for the display to work
// Should be called before any other functios are
void display_initialize();

// Terminates the display in a proper manner
void display_terminate();

// Clears the display to black
void display_clear();

// Updates a single pixel in the buffer
// call display_display() to display changes
void display_update(int x, int y, byte flag);

// Displayes the image
void display_show();

#endif	// !PS_DISPLAY