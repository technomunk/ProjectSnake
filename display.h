/*
	A class of high level OLED display based functions
	
	all methods have display_ prefix, similar to Display:: syntax in higher level languages
	
	most methods DON'T perform argument legality checks, so they may break if you screw up
*/

#ifndef _PS_DISPLAY
#define _PS_DISPLAY

#include "utils.h"
#include "time.h"
#include "font.h"


#define PSD_PORT_VDD PORTF
#define PSD_MASK_VDD 0x40

#define PSD_PORT_VBATT PORTF
#define PSD_MASK_VBATT 0x20

#define PSD_PORT_DATA_COMMAND PORTF
#define PSD_MASK_DATA 0x10

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

/*
	Initializes everything necessary for the display to work
	Should be called before any other functios are
	
	Requires time_initialize() and spi_initialize() to be called prior in order to function
*/
void display_initialize();

/*
	Terminates the display in a proper manner
*/
void display_terminate();

/*
	Clears the display buffer to black
	to clear the display use display_show() right after
*/
void display_clearBuffer();

/*
	Inverts the whole buffer
*/
void display_invertBuffer();

/*
	Updates a single pixel in the buffer
	Does NOT update the display
	Use this with display_show() afterwards if you need to set up a multitude of pixels
	
	Arguments:
		int		- horizontal pixel coordinate
		int		- vertical pixel coordinate
		int		- boolean value to which set the pixel
*/
void display_put(int x, int y, int flag);

/*
	Updates a single pixel on the display
	This function is slower than display_put() on its own, but faster than a display_put() + display_show()
	
	Arguments:
		int		- horizontal pixel coordinate
		int		- vertical pixel coordinate
		int		- boolean value to which set the pixel
*/
void display_update(int x, int y, int flag);

/*
	Displayes the buffered image onto the screen
*/
void display_show();

/*
	Clears a rectange in the buffer
	to clear it on display call display_show() or display_updateRect() right after
	
	Arguments:
		int		- horizontal coordinate of left top corner
		int		- vertical coordinate of left top corner
		int		- width of the rectange
		int		- height of the rectangle
*/
void display_clearRect(int x, int y, int w, int h);

/*
	Displays a rectange from the buffer on the display
	Will use minimal amount of data transfer
	
	Arguments:
		int		- horizontal coordinate of left top corner
		int		- vertical coordinate of left top corner
		int		- width of the rectange
		int		- height of the rectangle
*/
void display_showRect(int x, int y, int w, int h);

/*
	Puts the given string into the buffer
	Call display_show() to display it on the screen
	
	Arguments:
		int 	- horizontal coordinate of left top corner
		int		- vertical coordinate of left top corner
		char *	- pointer to a null-terminated string of characters to display
*/
void display_putString(int x, int y, char * pString);

/*
	Changes display brightness level
	
	Arguments:
		byte	- new brightness level
*/
void display_setBrightness(byte brightness);


#endif	// !_PS_DISPLAY