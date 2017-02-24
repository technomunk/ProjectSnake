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
	42
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

const unsigned char speed_leds[] = {
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

void loop_menu() {
	unsigned char buttons = input_getButtons();
	
	if (buttons)
		usedButtons = buttons;
	
	if (buttons) {
		if ((usedButtons & 1) || (usedButtons & 8)) {
			if ((time_tick - vars.menu.lastSelect) >= 20) {
				vars.menu.selected = !vars.menu.selected;
				
				if (vars.menu.selected) {
					display_clearRect(PSD_DISPLAY_WIDTH / 2 - 32, 20, 8, 8);
					display_putChar(PSD_DISPLAY_WIDTH / 2 - 24, 4, PSF_CHAR_SELECTOR_LEFT);
				} else {
					display_clearRect(PSD_DISPLAY_WIDTH / 2 - 24, 4, 8, 8);
					display_putChar(PSD_DISPLAY_WIDTH / 2 - 32, 20, PSF_CHAR_SELECTOR_LEFT);
				}
				
				display_showRect(PSD_DISPLAY_WIDTH / 2 - 32, 20, 8, 8);
				display_showRect(PSD_DISPLAY_WIDTH / 2 - 24, 4, 8, 8);
				
				vars.menu.lastSelect = time_tick;
			}
		} else {
			setState((vars.menu.selected) ? PS_STATE_GAME : PS_STATE_MENU);
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
	PORTE = speed_leds[vars.game.speed];
	
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
	
	if (time_tick - stateBeginTime <= 160) {
		if (((time_tick - stateBeginTime) % 20) == 0) {
			display_invert();
			display_show();
		}
	} else {
		if (vars.over.substate) {
			if (buttons) {
				if ((buttons & 1) || (buttons & 8)) {
					if ((time_tick - vars.over.lastSelect) >= 20) {
						vars.over.selected = !vars.over.selected;
						
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
					}
				} else {
					setState((vars.over.selected) ? PS_STATE_MENU : PS_STATE_GAME);
				}
			}
		} else {
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
		}
	}
	
	lastTime = time_tick;
}

void loop_high() {
	if (lastTime == time_tick)
		return;
	
	display_clear();
	display_putString(0, 0, "New Highscore!");
	display_show();
	
	lastTime = time_tick;
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
	switch (s) {
	case PS_STATE_GAME:
		game_start(PS_SNAKE_SIZE, input_getSwitches());
		loop = &loop_game;
	break;
	
	case PS_STATE_OVER:
	case PS_STATE_HIGH:
		vars.over.lastSelect = time_tick;
		vars.over.substate = 0;
		int i;
		unsigned int highscore = 0;
		for (i = PS_SCORE_COUNT; i > 0; --i)
			if (score_scores[i] < game_score) {
				highscore = 1;
				break;
			}
		loop = (highscore) ? &loop_high : &loop_over;
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
		
		vars.menu.lastSelect = 0;
		vars.menu.selected = 0;
		
		loop = &loop_menu;
	break;
	}
	stateBeginTime = time_tick;
}