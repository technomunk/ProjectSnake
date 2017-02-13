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

#include <pic32mx.h>

#include "display.h"



// ===================
// === DEFINITIONS ===
// ===================

#define SNAKE_SIZE 8

extern void loop();

int bLooping = 1;
int x = 0, flag = 1;

// ===================
// === Actual code ===
// ===================

// Program entry
int main() {
	
	// INITIALIZATION
	spi_initialize();
	display_initialize();
	
	display_clear();
	
	// MAIN LOOP
	while (bLooping)
		loop();
	
	// TERMINATION
	display_terminate();
	
	return 0;
}

// Core loop of the program
void loop() {
	int y;
	for (y = 0; y < 32; y++)
		display_update(x, y, flag);
	display_show();
	if (++x == 128) {
		x = 0;
		flag = !flag;
	}
	delay(150000);
}