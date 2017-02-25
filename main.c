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

#define PS_SNAKE_SIZE	8

#define PS_STATE_MENU	0
#define PS_STATE_GAME	1
#define PS_STATE_OVER	2
#define PS_STATE_HIGH	3
#define PS_STATE_SCORE	4
#define PS_STATE_IDLE	5

#define PS_BLINK_PERIOD	20
#define PS_INPUT_PAD	20

typedef unsigned char state;

void setState(state);

unsigned int lastTime = 0, stateBeginTime = 0;
unsigned char usedButtons;

// Pointer to loop function
void (*loop)() = 0;

#define PS_SCORE_COUNT 9

char score_names[PS_SCORE_COUNT][4] = {
	"MAX",
	"Liz",
	"Ela",
	
	"Jak",
	"Dan",
	"Oly",
	
	"Fox",
	"Bob",
	"Fun"
};

unsigned int score_scores[PS_SCORE_COUNT] = {
	15000,
	8654,
	5751,
	
	2642,
	1242,
	845,
	
	385,
	124,
	0
};

// State dependent variables live in this struct
union Variables {
	struct Menu {
		unsigned char selected;
		unsigned int lastSelect;
	} menu;
	
	struct Game {
		unsigned int speed;
		unsigned char lastButtons;
		direction dir;
	} game;
	
	struct Over {
		unsigned char substate;
		unsigned char selected;
		unsigned int lastSelect;
		unsigned char highscore;
	} over;
	
	struct High {
		char * name;
		unsigned char selected;
		unsigned char blink;
		unsigned int scorePos;
		unsigned int lastSelect;
	} high;
	
	struct Score {
		
	} score;

	struct Idle {
	} idle;
} vars;

const unsigned int speed_scales[] = {
	40,
	20,
	10,
	8,
	5,
	4,
	2,
	1
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
	PORTE = 0;				// clear LEDS
	
	display_setBrightness(0x7F);
	
	setState(PS_STATE_MENU);
	
	// MAIN LOOP
	while (loop)
		loop();
	
	// TERMINATION
	display_terminate();
	
	return 0;
}

// =======================
// === Different loops ===
// =======================

void loop_menu() {
	unsigned char buttons = input_getButtons();
	
	if (buttons) 
		usedButtons = buttons;
	
	if (buttons && ((time_tick - vars.menu.lastSelect) >= PS_INPUT_PAD)) {
		if ((usedButtons & 1) || (usedButtons & 8)) {
			if (vars.menu.selected) {
				display_clearRect(PSD_DISPLAY_WIDTH / 2 - 32, 20, 8, 8);
				display_putChar(PSD_DISPLAY_WIDTH / 2 - 24, 4, PSF_CHAR_SELECTOR_LEFT);
			} else {
				display_clearRect(PSD_DISPLAY_WIDTH / 2 - 24, 4, 8, 8);
				display_putChar(PSD_DISPLAY_WIDTH / 2 - 32, 20, PSF_CHAR_SELECTOR_LEFT);
			}
			
			display_showRect(PSD_DISPLAY_WIDTH / 2 - 32, 20, 8, 8);
			display_showRect(PSD_DISPLAY_WIDTH / 2 - 24, 4, 8, 8);
			
			
			vars.menu.selected = !vars.menu.selected;
			vars.menu.lastSelect = time_tick;
		} else {
			setState((vars.menu.selected) ? PS_STATE_SCORE : PS_STATE_GAME);
		}
	}
	
	lastTime = time_tick;
}

void loop_game() {
	unsigned char buttons = input_getButtons();
	
	if (buttons)
		usedButtons = buttons;
	
	if (time_tick == lastTime)
		return;
	
	vars.game.speed = input_getDial() / 128;
	PORTE = 255 - (127 >> vars.game.speed);
	
	lastTime = time_tick;
	
	if (time_tick % 20 == 0)
		game_blinkOneUp();
	
	// Update the game only once per speed_scalse
	if ((time_tick % speed_scales[vars.game.speed]) != 0)
		return;
	
	if (!game_updateWalls(input_getSwitches()))
		setState(PS_STATE_OVER);
	
	if (vars.game.lastButtons != usedButtons) {
		vars.game.lastButtons = usedButtons;
		if (usedButtons & 8)
			vars.game.dir = PSG_DIR_LEFT;
		else if (usedButtons & 1)
			vars.game.dir = PSG_DIR_RIGHT;
		else if (usedButtons & 2)
			vars.game.dir = PSG_DIR_DOWN;
		else
			vars.game.dir = PSG_DIR_UP;
	}
	
	game_score_multiplier = speed_scales[(sizeof(speed_scales) / sizeof(speed_scales[0])) - vars.game.speed - 1];
	if (!game_update(vars.game.dir))
		setState(PS_STATE_OVER);
	
	lastTime = time_tick;
}

void loop_over() {
	unsigned char buttons = input_getButtons();
	
	if (buttons)
		usedButtons = buttons;
	
	if (time_tick == lastTime)
		return;
	
	if (time_tick - stateBeginTime <= 140) {
		// Play a simple animation
		if (((time_tick - stateBeginTime) % PS_BLINK_PERIOD) == 0) {
			display_invert();
			display_show();
		}
	} else {
		// This will make sure we only update the screen once
		if (!vars.over.substate) {
			int i;
			// Check if the new score is a highscore
			for (i = 0; i < PS_SCORE_COUNT; i++)
				if (score_scores[i] < game_score) {
					// Cheat big time
					vars.high.name = score_names[i];
					vars.high.scorePos = i;
					setState(PS_STATE_HIGH);
					return;
				}
			// Show the actual screen
			display_clear();
			int size;
			char * str = intToStr(game_score, &size);
			display_putString((PSD_DISPLAY_WIDTH - (size * PSF_CHAR_WIDTH)) / 2, 2, str);
			display_putChar(PSD_DISPLAY_WIDTH / 2 - 28, 12, PSF_CHAR_SELECTOR_LEFT);
			display_putString(PSD_DISPLAY_WIDTH / 2 - 20, 12, "AGAIN");
			display_putString(PSD_DISPLAY_WIDTH / 2 - 16, 22, "STOP");
			vars.over.selected = 0;
			display_show();
			vars.over.substate = 1;
		} else {
			// Respont to input
			if (buttons && ((time_tick - vars.over.lastSelect) >= PS_INPUT_PAD)) {
				if ((usedButtons & 1) || (usedButtons & 8)) {
					vars.over.selected = !(vars.over.selected);
					
					if (vars.over.selected) {
						display_clearRect(PSD_DISPLAY_WIDTH / 2 - 28, 12, 8, 8);
						display_putChar(PSD_DISPLAY_WIDTH / 2 - 24, 22, PSF_CHAR_SELECTOR_LEFT);
					} else {
						display_clearRect(PSD_DISPLAY_WIDTH / 2 - 24, 22, 8, 8);
						display_putChar(PSD_DISPLAY_WIDTH / 2 - 28, 12, PSF_CHAR_SELECTOR_LEFT);
					}
					
					display_showRect(PSD_DISPLAY_WIDTH / 2 - 24, 22, 8, 8);
					display_showRect(PSD_DISPLAY_WIDTH / 2 - 28, 12, 8, 8);
					
					vars.over.lastSelect = time_tick;
				} else {
					setState((vars.over.selected) ? PS_STATE_MENU : PS_STATE_GAME);
				}
			}
			
		}
	}
	
	lastTime = time_tick;
}

void showSelection() {
	switch (vars.high.selected) {
	case 0:
		display_putChar((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, vars.high.name[vars.high.selected]);
		display_showRect((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
	break;
	
	case 1:
		display_putChar((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, vars.high.name[vars.high.selected]);
		display_showRect((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
	break;
	
	case 2:
		display_putChar((PSD_DISPLAY_WIDTH + PSF_CHAR_WIDTH) / 2, 16, vars.high.name[vars.high.selected]);
		display_showRect((PSD_DISPLAY_WIDTH + PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
	break;
	
	default:
		display_putString((PSD_DISPLAY_WIDTH - (4 * PSF_CHAR_WIDTH)) / 2, 24, "Save");
		display_showRect((PSD_DISPLAY_WIDTH - (4 * PSF_CHAR_WIDTH)) / 2, 24, PSF_CHAR_WIDTH * 4, PSF_CHAR_HEIGHT);
	break;
	}
}

void loop_high() {
	unsigned char buttons = input_getButtons();
	
	if (buttons)
		usedButtons = buttons;
	
	if (lastTime == time_tick)
		return;
	
	lastTime = time_tick;
	
	// Make sure the player has time no notice change before reading inputs
	if (time_tick - stateBeginTime < PS_INPUT_PAD)
		return;
	
	if (((time_tick - stateBeginTime) % PS_BLINK_PERIOD) == 0) {
		switch (vars.high.selected) {
		case 0:
			// First letter of name
			if (vars.high.blink)
				display_putChar((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, vars.high.name[0]);
			else
				display_clearRect((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
			display_showRect((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
		break;
		
		case 1:
			// Second letter of name
			if (vars.high.blink)
				display_putChar((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, vars.high.name[1]);
			else
				display_clearRect((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
			display_showRect((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
		break;
			
		case 2:
			// Third letter of name
			if (vars.high.blink)
				display_putChar((PSD_DISPLAY_WIDTH + PSF_CHAR_WIDTH) / 2, 16, vars.high.name[2]);
			else
				display_clearRect((PSD_DISPLAY_WIDTH + PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
			display_showRect((PSD_DISPLAY_WIDTH + PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
		break;
			
		case 3:
			// Save
			if (vars.high.blink)
				display_putString((PSD_DISPLAY_WIDTH - (4 * PSF_CHAR_WIDTH)) / 2, 24, "Save");
			else
				display_clearRect((PSD_DISPLAY_WIDTH - (4 * PSF_CHAR_WIDTH)) / 2, 24, PSF_CHAR_WIDTH * 4, PSF_CHAR_HEIGHT);
			display_showRect((PSD_DISPLAY_WIDTH - (4 * PSF_CHAR_WIDTH)) / 2, 24, PSF_CHAR_WIDTH * 4, PSF_CHAR_HEIGHT);
		break;
		};
		
		vars.high.blink = !(vars.high.blink);
	}
	
	// Respond to input
	if (buttons && ((time_tick - vars.high.lastSelect) >= PS_INPUT_PAD)) {
		if (usedButtons & 8) {
			if (vars.high.blink)
				showSelection();
			
			// Go left
			if (vars.high.selected == 0)
				vars.high.selected = 3;
			else
				vars.high.selected--;
			
		} else if (usedButtons & 1) {
			if (vars.high.blink)
				showSelection();
			
			// Go right
			if (vars.high.selected == 3)
				vars.high.selected = 0;
			else
				vars.high.selected++;
		} else {
			if (vars.high.selected == 3) {
				// Save high score
				score_scores[vars.high.scorePos] = game_score;
				// Name is taken care of because we dereference its position
				setState(PS_STATE_MENU);
				return;	// An important return
			} else {
				// Check which direction we're going
				if (usedButtons & 4) {
					// Go up
					if (vars.high.name[vars.high.selected] == 'A')
						vars.high.name[vars.high.selected] = 'z';
					else if (vars.high.name[vars.high.selected] == 'a')
						vars.high.name[vars.high.selected] = 'Z';
					else
						vars.high.name[vars.high.selected]--;
				} else {
					// Go down
					if (vars.high.name[vars.high.selected] == 'Z')
						vars.high.name[vars.high.selected] = 'a';
					else if (vars.high.name[vars.high.selected] == 'z')
						vars.high.name[vars.high.selected] = 'A';
					else
						vars.high.name[vars.high.selected]++;
				}
				showSelection();
			}
		}
		vars.high.lastSelect = time_tick;
	}	
};

void loop_score() {
	if (lastTime == time_tick)
		return;
	
	display_clear();
	display_putString(0, 0, "Score");
	display_show();
	
	lastTime = time_tick;
}

void loop_idle() {
	if (lastTime == time_tick)
		return;
	
	display_clear();
	display_putString(0, 0, "Idle");
	display_show();
	
	lastTime = time_tick;
}

void setState(state s) {
	PORTE = 0;
	switch (s) {
	case PS_STATE_GAME:
		game_start(PS_SNAKE_SIZE, input_getSwitches());
		loop = &loop_game;
	break;
	
	case PS_STATE_OVER:
		vars.over.lastSelect = time_tick;
		vars.over.substate = 0;
		int i;
		loop = &loop_over;
	break;
	
	case PS_STATE_HIGH:
		// Show the new highscore!
		display_clear();
		display_putString(8, 0, "New Highscore!");
		int size;
		char * str = intToStr(game_score, &size);
		display_putString((PSD_DISPLAY_WIDTH - (size * PSF_CHAR_WIDTH)) / 2, 8, str);
		vars.high.selected = 0;
		vars.high.blink = 0;
		// The name has been taken care of in over
		display_putString((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, vars.high.name);
		display_putString((PSD_DISPLAY_WIDTH - (4 * PSF_CHAR_WIDTH)) / 2, 24, "Save");
		display_show();
		vars.high.lastSelect = time_tick;
		loop = &loop_high;
	break;
	
	case PS_STATE_SCORE:
		display_clear();
		loop = &loop_score;
	break;
	
	case PS_STATE_IDLE:
		display_clear();
		loop = &loop_idle;
	break;
	
	default:
		display_clear();
		display_putString(PSD_DISPLAY_WIDTH / 2 - 16, 4, "PLAY");
		display_putString(PSD_DISPLAY_WIDTH / 2 - 24, 20, "SCORES");
		display_putChar(PSD_DISPLAY_WIDTH / 2 - 24, 4, PSF_CHAR_SELECTOR_LEFT);
		display_show();
		
		vars.menu.lastSelect = time_tick;
		vars.menu.selected = 0;
		
		loop = &loop_menu;
	break;
	}
	stateBeginTime = time_tick;
}