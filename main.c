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

// Game includes all we need
// But everything has define guards, so you can still include them here
#include "game.h"

// ===================
// === DEFINITIONS ===
// ===================

#define SNAKE_SIZE 8

extern void loop();

int bLooping = 1;
unsigned int speed = 0, lastTime = 0;

unsigned char lastButtons, usedButtons;
direction dir = PSG_DIR_UP;

void gameover() {
	game_start(8, input_getSwitches());
}

unsigned int speed_scales[] = {
	40,
	20,
	10,
	8,
	5,
	4,
	2,
	1
};

unsigned char speed_leds[] = {
	0x80,
	0xC0,
	0xE0,
	0xF0,
	0xF8,
	0xFC,
	0xFE,
	0xFF
};

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
	input_initialize();
	
	display_clear();
	display_show();
	
	TRISECLR = 0xFF;		// preapare LEDS
	PORTE = 0;
	
	display_setBrightness(0x7F);
	
	game_start(8, input_getSwitches());
	
	// MAIN LOOP
	while (bLooping)
		loop();
	
	// TERMINATION
	display_terminate();
	
	return 0;
}

int x = 127;

// Core loop of the program
void loop() {	
	unsigned char buttons = input_getButtons();
	
	if (buttons)
		usedButtons = buttons;
	
	// Quit early, because we don't want to update these parts too often
	if (time_tick == lastTime)
		return;
	
	speed = input_getDial() / 128;
	PORTE = speed_leds[speed];
	
	lastTime = time_tick;
	
	if (time_tick % 20 == 0)
		game_blinkOneUp();
	
	// Update the game only once per speed_scalse
	if (time_tick % speed_scales[speed] != 0)
		return;
	
	if (!game_updateWalls(input_getSwitches()))
			gameover();
	
	if (lastButtons != usedButtons) {
		lastButtons = usedButtons;
		if (usedButtons & 8)
			dir = PSG_DIR_LEFT;
		else if (usedButtons & 1)
			dir = PSG_DIR_RIGHT;
		else if (usedButtons & 2)
			dir = PSG_DIR_DOWN;
		else
			dir = PSG_DIR_UP;
	}
	
	game_score_multiplier = speed_scales[(sizeof(speed_scales) / sizeof(speed_scales[0])) - speed - 1];
	if (!game_update(dir))
		gameover();
	lastTime = time_tick;
}