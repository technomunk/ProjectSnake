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
	
	
	PSD_PORT_DATA_COMMAND &= ~PSD_MASK_DATA;	// set spi as commands
	PSD_PORT_VDD &= ~PSD_MASK_VDD;				// reset voltage
	
	// Display off
	spi_byte(PSD_CMD_DISPLAY_OFF);
	
	// Reset the display
	PSD_PORT_RESET &= ~PSD_MASK_RESET;
	time_wait(1);
	PSD_PORT_RESET |= PSD_MASK_RESET;
	
	// Set charge pump
	spi_byte(PSD_CMD_SET_CHARGE_PUMP);
	spi_byte(0x14);
	
	// Set Pre-charge period
	spi_byte(PSD_CMD_SET_PRE_CHARGE);
	spi_byte(0xF1);
	
	// Apply voltage
	PSD_PORT_VBATT &= PSD_MASK_VBATT;
	time_wait(100);
	
	// Remap the display so that the origin is in left top corner
	spi_byte(PSD_CMD_MAP_COLUMNS);	// remap columns
	spi_byte(PSD_CMD_MAP_ROWS);		// remap rows
	
	// COM (decoding) settings
	spi_byte(PSD_CMD_SET_COM);
	spi_byte(PSD_COM_LEFT_TOP);
	
	// Memory adressing mode
	spi_byte(PSD_CMD_SET_ADDRESS_MODE);
	spi_byte(PSD_ADDRESS_MODE_HORIZONTAL);
	
	// Display on
	spi_byte(PSD_CMD_DISPLAY_ON);
}

/*
	Terminates the display in a proper manner
	Don't think it's actually used, but thats what documentation calls for
	======================================================================
	by
		Grigory Glukhov
*/
void display_terminate() {
	// Display off
	spi_byte(PSD_CMD_DISPLAY_OFF);
	
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
	Inverts the whole buffer
	========================
	by
		Grigory Glukhov
*/
void display_invert() {
	
	int id;
	
	for (id = 0; id < PSD_SIZE_BUFFER; id++)
		display_buffer[id] = ~display_buffer[id];
}

/*
	Updates part of display buffer
	Does NOT change the displayed image
	call display_show() for that
	===================================
	by
		Grigory Glukhov
*/
void display_put(int x, int y, int flag) {
	
#if PSD_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
#endif	// PSD_VALIDATE_ARGS
	
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
void display_update(int x, int y, int flag) {
	
#if PSD_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
#endif	// PSD_VALIDATE_ARGS
	
	PSD_PORT_DATA_COMMAND &= ~PSD_MASK_DATA;
	
	int page = y / 8;
	
	// Send page number here
	spi_byte(PSD_CMD_SET_PAGE_UNSAFE(page));	// use unsafe, if VALIDATE_ARGS it is safe, if not - not supposed to be
	
	// Send row number
	spi_byte(PSD_CMD_SET_COL_LOW(x));	// low nibble
	spi_byte(PSD_CMD_SET_COL_HIGH_UNSAFE(x));	// high nibble	// use unsafe, if VALIDATE_ARGS it is safe, if not - not supposed to be
	
	
	page *= PSD_COLUMN_COUNT;
	y %= 8;
	
	if (flag)
		display_buffer[page + x] |= (1 << y);
	else
		display_buffer[page + x] &= ~(1 << y);
	
	
	PSD_PORT_DATA_COMMAND |= PSD_MASK_DATA;
	spi_byte(display_buffer[page + x]);
}

/*
	Sends a single change brightness command to the display
	=======================================================
	by
		Grigory Glukhov
*/
void display_setBrightness(byte b) {
	PSD_PORT_DATA_COMMAND &= ~PSD_MASK_DATA;
		
	spi_byte(PSD_CMD_SET_BRIGHTNESS);
	spi_byte(b);
}

/*
	Clears a rectangle in the buffer
	Function is not pretty, but that is a consequence of map-independent rectangle clearing
	Might not be necessary for the game, but Greg made this while fooling around with the display
	================================
	by
		Grigory Glukhov
*/
void display_clearRect(int x, int y, int w, int h) {
	
#if PSD_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
	
	if (x + w > PSD_DISPLAY_WIDTH)
		w = PSD_DISPLAY_WIDTH - x;
	
	if (y + h > PSD_DISPLAY_HEIGHT)
		h = PSD_DISPLAY_HEIGHT - y;
#endif	// PSD_VALIDATE_ARGS
	
	byte col = 1;
	
	int i, yo, page = (y / 8) * PSD_COLUMN_COUNT;
	
	w += x;
	
	y %= 8;
	
	if (h + y <= 8) {
		col = 1;
		for (i = 1; i < h; i++) {
			col = col << 1;
			col |= 1;
		}
		
		col = col << y;
		col = ~col;
		
		for (i = x; i < w; i++)
			display_buffer[page + i] &= col;
		
		return;
	} else {
		col = 0xFF;
		col = col << y;
		
		col = ~col;
		
		for (i = x; i < w; i++)
			display_buffer[page + i] &= col;
		
		h -= 8 - y;
		
		while (h > 0) {
			page += PSD_COLUMN_COUNT;
			
			if (h > 8)
				col = 0;
			else
				col = ~(0xFF >> (8 - h));
			
			for (i = x; i < w; i++)
				display_buffer[page + i] &= col;
			
			h -= 8;
		}
	}
}

/*
	Updates minimal possible rectange of the screen which always contains the provided one
	======================================================================================
	by
		Grigory Glukhov
*/
void display_showRect(int x, int y, int w, int h) {
	
#if PSD_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
	
	if (x + w > PSD_DISPLAY_WIDTH)
		w = PSD_DISPLAY_WIDTH - x;
	
	if (y + h > PSD_DISPLAY_HEIGHT)
		h = PSD_DISPLAY_HEIGHT - y;
#endif	// PSD_VALIDATE_ARGS
	
	h += y;
	
	if (h % 8 == 0)
		h /= 8;
	else
		h = h / 8 + 1;

	y /= 8;
	
	PSD_PORT_DATA_COMMAND &= ~PSD_MASK_DATA;
	
	// Reset column
	spi_byte(PSD_CMD_SET_COLUMN);		// send set column command
	spi_byte(x);		// starting column
	spi_byte(x + w - 1);	// ending column
	
	// Reset page
	spi_byte(PSD_CMD_SET_PAGE);		// send set row command
	spi_byte(y);		// starting page
	spi_byte(h - 1);		// ending page

	PSD_PORT_DATA_COMMAND |= PSD_MASK_DATA;
	
	int i;
	for (i = y; i < h; i++)
		spi_array(display_buffer + (i * PSD_COLUMN_COUNT) + x, w);
}

void display_putString(int x, int y, char * pString) {
	
#if PSD_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
#endif	// PSD_VALIDATE_ARGS
	
	if (y % 8) {
		
		// do magic
		int rt = y % 8;
		int rb = 8 - rt;
		y = (y / 8) * PSD_COLUMN_COUNT;
		int yb = y + PSD_COLUMN_COUNT;
		
		int i;
		// Do until null char (0x0)
		while (*pString) {
			for (i = 0; i < PSF_CHAR_WIDTH; i++) {
#if PSD_VALIDATE_ARGS
				if (x + i >= PSD_DISPLAY_WIDTH)
					break;
				else if (y + PSF_CHAR_HEIGHT)
					break;
#endif	// PSD_VALIDATE_ARGS
				
#if PSF_CHECK_STRING
				display_buffer[y + x] |= font[((*pString) % PSF_CHAR_COUNT) * PSF_CHAR_WIDTH + i] << rt;
				display_buffer[yb + x] |= font[((*pString) % PSF_CHAR_COUNT) * PSF_CHAR_WIDTH + i] >> rb;
#else
				display_buffer[y + x] |= font[(*pString) * PSF_CHAR_WIDTH + i] << rt;
				display_buffer[yb + x] |= font[(*pString) * PSF_CHAR_WIDTH + i] >> rb;
#endif	// PSF_CHECK_STRING
				x++;
			}
			pString++;
		}
		
	} else {
		
		// simple case
		y = (y / 8) * PSD_COLUMN_COUNT;
		int i;
		// Do until null char (0x0)
		while (*pString) {
			for (i = 0; i < PSF_CHAR_WIDTH; i++) {
#if PSD_VALIDATE_ARGS
				if (x + i >= PSD_DISPLAY_WIDTH)
					break;
				else if (y + PSF_CHAR_HEIGHT)
					break;
#endif	// PSD_VALIDATE_ARGS
#if PSF_CHECK_STRING
				display_buffer[y + x] |= font[((*pString) % PSF_CHAR_COUNT) * PSF_CHAR_WIDTH + i];
#else
				display_buffer[y + x] |= font[(*pString) * PSF_CHAR_WIDTH + i];
#endif	// PSF_CHECK_STRING
				x++;
			}
			pString++;
		}
	}
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
	
	PSD_PORT_DATA_COMMAND &= ~PSD_MASK_DATA;
	
	// Reset column
	spi_byte(PSD_CMD_SET_COLUMN);	// send set column command
	spi_byte(0);					// starting column
	spi_byte(127);					// ending column
	
	// Reset page
	spi_byte(PSD_CMD_SET_PAGE);		// send set row command
	spi_byte(0);					// starting page
	spi_byte(3);					// ending page

	PSD_PORT_DATA_COMMAND |= PSD_MASK_DATA;
	
	spi_array(display_buffer, PSD_SIZE_BUFFER);
		
}