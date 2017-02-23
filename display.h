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


// false	- maximize performance, but functions become unsafe (access violations may occur on illegal arguments and so on)
// true 	- sacrifice some performance, but make functions safe (cast illegal args into legal onces, bail early on illegal cases)
#define PSD_VALIDATE_ARGS 0


// This is SPI commands for the display
// I got them at https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
//
// As I'm too lazy to copy the stuff I don't use this will lack some of the commands

#define PSD_CMD_DISPLAY_ON				0xAF
#define PSD_CMD_DISPLAY_OFF				0xAE

#define PSD_CMD_SET_CHARGE_PUMP			0x8D
#define PSD_CMD_SET_PRE_CHARGE			0xD9

#define PSD_CMD_SET_COM					0xDA
#define PSD_COM_LEFT_TOP				0x20	// this actually means, row 0 is top and allow column remap

#define PSD_CMD_SET_ADDRESS_MODE		0x20
#define PSD_ADDRESS_MODE_HORIZONTAL		0x00	// from left to right, wrap around at right
#define PSD_ADDRESS_MODE_VERTICAL		0x01	// from top to bottom, wrap around at bottom
#define PSD_ADDRESS_MODE_PAGE			0x02	// from left to right

// Tri-byte command (send starting column after and then ending column)
#define PSD_CMD_SET_COLUMN				0x21
// Tri-byte command (send starting page after and then ending page)
#define PSD_CMD_SET_PAGE				0x22

// Picked this from https://github.com/is1200-example-projects/hello-display
// As official documentation is too dark about this exact command
#define PSD_CMD_MAP_COLUMNS				0xA1
// Another fake, as this actually means COM horizontal mapping mode and depends on COM_MODE to be 0x2x to function
#define PSD_CMD_MAP_ROWS				0xC8

// Not actually used, because if VALIDATE_ARGS is true then we will make sure we use safe values
// Otherwise we're not interested in safe, we're interested in fast
#define PSD_CMD_SET_PAGE_SAFE(x)		(0xB0 | (x & 0xF))
#define PSD_CMD_SET_PAGE_UNSAFE(x)		(0xB0 | x)

#define PSD_CMD_SET_COL_LOW(x)			(x & 0xF)

// Not actually used, because if VALIDATE_ARGS is true then we will make sure we use safe values
// Otherwise we're not interested in safe, we're interested in fast
#define PSD_CMD_SET_COL_HIGH_SAFE(x)	(0x10 | ((x >> 4) & 0xF))
#define PSD_CMD_SET_COL_HIGH_UNSAFE(x)	(0x10 | (x >> 4))

#if PSD_VALIDATE_ARGS
#define PSD_CMD_SET_COL_HIGH(x)			PSD_CMD_SET_COL_HIGH_SAFE(x)
#define PSD_CMD_SET_PAGE_SINGLE(x)		PSD_CMD_SET_PAGE_SAFE(x)
#else
#define	PSD_CMD_SET_COL_HIGH(x)			PSD_CMD_SET_COL_HIGH_UNSAFE(x)
#define PSD_CMD_SET_PAGE_SINGLE(x)		PSD_CMD_SET_PAGE_UNSAFE(x)
#endif	// PSD_VALIDATE_ARGS

#define PSD_CMD_SET_BRIGHTNESS			0x81

#define PSD_PORT_VDD					PORTF
#define PSD_MASK_VDD					0x40

#define PSD_PORT_VBATT					PORTF
#define PSD_MASK_VBATT					0x20

#define PSD_PORT_DATA_COMMAND			PORTF
#define PSD_MASK_DATA					0x10

#define PSD_PORT_RESET					PORTG
#define PSD_MASK_RESET					0x200


#define PSD_DISPLAY_WIDTH		128
#define PSD_DISPLAY_HEIGHT		32
#define PSD_PAGE_COUNT			4
#define PSD_COLUMN_COUNT		128	// theoretically could be different from PSD_DISPLAY_WIDTH in different mapping conditions, but not in our case
#define PSD_SIZE_BUFFER			(PSD_PAGE_COUNT * PSD_COLUMN_COUNT)

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