/*
	A class of high level OLED display based functions
	
	all methods have display_ prefix, similar to Display:: syntax in higher level languages
	
	methods addressing the display buffer (display_put() and display_update()) perform no legality checks, make sure you're not out of array bounds when calling
*/

#ifndef PS_DISPLAY
#define PS_DISPLAY

#include "utils.h"
#include "time.h"


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
#define PSD_SIZE_BUFFER (PSD_PAGE_COUNT * PSD_COLUMN_COUNT)

//	Buffer of display data
//	Avoid adressing directly
//
//	dev comment: use single dimensional buffer address to speed up writing to it, sacrificing some of the readability and single_pixel addressing
extern byte display_buffer[PSD_SIZE_BUFFER];

//	Initializes everything necessary for the display to work
//	Should be called before any other functios are
void display_initialize();

//	Terminates the display in a proper manner
void display_terminate();

//	Clears the display buffer to black
//	to clear the display use display_show() right after
void display_clearBuffer();

//	Updates a single pixel in the buffer
//	Does NOT update the display
//	Use this with display_show() afterwards if you need to set up a multitude of pixels
void display_put(int x, int y, byte flag);

//	Displayes the buffered image onto the screen
void display_show();

//	Updates a single pixel on the display
void display_update(int x, int y, byte flag);

//	Changes display brightness level
void display_setBrightness(byte brightness);


#endif	// !PS_DISPLAY