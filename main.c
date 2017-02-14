/*
	Snake Game Project
	
	for Computer Hardware Engineering course at KTH
	
	by
	Grigory Glukhov 
	Sasa Lekic 
	
	Spring 2017
	
	============================================================
	
	Publically available at  https://github.com/Griffone/ProjectSnake
	
	You are free to use this code at your own risk!
	
*/

// ================
// === INCLUDES ===
// ================

#include <pic32mx.h>

#include "display.h"



// ===================
// === DEFINITIONS ===
// ===================

#define SNAKE_SIZE 8

extern void loop();

int bLooping = 1;
int x = 0, y = 0, dirX = 1, dirY = 1;
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
	
	for (x = 0; x < 128; x++) {
		display_update(x, 0, 1);
		display_update(x, 31, 1);
	}
	
	for (y = 1; y < 31; y++) {
		display_update(0, y, 1);
		display_update(127, y, 1);
	}
	
	x = 0;
	y = 0;
	
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
	
	display_update(x, y, 0);
	
	x += dirX;
	y += dirY;
	// check horizontal movement
	if (x == 126)
		dirX = -1;
	else if (x == 1)
		dirX = 1;
	
	// check vertical movement
	if (y == 30)
		dirY = -1;
	else if (y == 1)
		dirY = 1;
	
	display_update(x, y, 1);
	display_show();
	lastTime = time_tick;
}