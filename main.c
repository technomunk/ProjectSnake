/*
	Snake Game Project
	
	for Computer Hardware Engineering course at KTH
	
	by
	Grigory Glukhov (960519-7434)
	Sasa Lekic (971104-T013)
	
	Spring 2017
	
	============================================================
	
	Publically available at  https://github.com/Griffone/ProjectSnake
	
	You are free to use this code at your own risk!
	
*/

// ================
// === INCLUDES ===
// ================

// Display includes all we need
// But everything has define guards, so you can still include them here
#include "display.h"



// ===================
// === DEFINITIONS ===
// ===================

#define SNAKE_SIZE 8

// Undefine first, so we don't get compiler warnings
#undef PSD_VALIDATE_ARGS
// Redefine as false to get MAXIMUM PERFORMANCE and showcase the cleanliness of game logic
#define PSD_VALIDATE_ARGS 0		// set this to false, because we're interested in maximum performance

extern void loop();

int bLooping = 1;
int x, y, dirX = 1, dirY = 1;
byte col = 0x00, colJmp = 0x20;
unsigned int lastTime = 0;

// ===================
// === Actual code ===
// ===================

// Program entry
int main() {
	
	// INITIALIZATION
	time_initialize();
	enable_interrupts();
	spi_initialize();
	display_initialize();
	
	display_clear();
	display_show();
	
	// set up borders
	display_invert();
	display_clearRect(1, 1, 126, 30);
	
	display_setBrightness(col);
	display_show();
	
	x = 1;
	y = 1;
	
	// MAIN LOOP
	while (bLooping)
		loop();
	
	// TERMINATION
	display_terminate();
	
	return 0;
}

// Core loop of the program
void loop() {
	if (time_tick == lastTime)
		return;
	
	// clear last position
	display_clearRect(x, y, 31, 8);
	
	x += dirX;
	y += dirY;
	// check horizontal movement
	if (x == 96 || x == 1) {
		dirX = -dirX;
		col += colJmp;
		if (col == 0xE0 || col == 0x0)
			colJmp = -colJmp;
	}
	
	// check vertical movement
	if (y == 23 || y == 1) {
		dirY = -dirY;
		col += colJmp;
		if (col == 0xE0 || col == 0x0)
			colJmp = -colJmp;
	}
	
	display_setBrightness(col);
	
	// set new position
	display_putString(x, y, "Bump");
	
	display_showRect(x - 1, y - 1, 33, 10);
	
	lastTime = time_tick;
}