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
#define PS_IDLE_TICK	800	// this is dependent on update interval


typedef enum {
	STATE_MENU,
	STATE_GAME,
	STATE_OVER,
	STATE_HIGH,
	STATE_SCORE,
	STATE_IDLE
} STATE;

#define PS_BLINK_PERIOD	20
#define PS_INPUT_PAD	20

void setState(STATE);

unsigned int lastTime = 0, stateBeginTime = 0;
unsigned char usedButtons;

// Pointer to loop function
void (*loop)() = 0;

#define PS_SCORE_PER_PAGE	((PSD_DISPLAY_HEIGHT / PSF_CHAR_HEIGHT) - 1)
#define PS_SCORE_PAGE_COUNT	3
#define PS_SCORE_COUNT		(PS_SCORE_PAGE_COUNT * PS_SCORE_PAGE_COUNT)

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
		char * name;
		unsigned char selected;
		unsigned char blink;
		unsigned int scorePos;
		unsigned int lastSelect;
	} high;
	
	struct Score {
		unsigned char page;
		unsigned int lastSelect;
	} score;

	struct Idle {
		int x;
		STATE lastState;
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
	
	setState(STATE_MENU);
	
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
			setState((vars.menu.selected) ? STATE_SCORE : STATE_GAME);
		}
	}
	
	if ((time_tick - vars.menu.lastSelect) > PS_IDLE_TICK) {
		vars.idle.lastState = STATE_MENU;
		setState(STATE_IDLE);
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
		setState(STATE_OVER);
	
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
		setState(STATE_OVER);
	
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
					setState(STATE_HIGH);
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
					
				} else {
					setState((vars.over.selected) ? STATE_MENU : STATE_GAME);
				}
				
				vars.over.lastSelect = time_tick;
			}
			
		}
	}
	
	lastTime = time_tick;
}

void showSelection() {
	switch (vars.high.selected) {
	case 0:
		display_clearRect((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
		display_putChar((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, vars.high.name[vars.high.selected]);
		display_showRect((PSD_DISPLAY_WIDTH - (3 * PSF_CHAR_WIDTH)) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
	break;
	
	case 1:
		display_clearRect((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
		display_putChar((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, vars.high.name[vars.high.selected]);
		display_showRect((PSD_DISPLAY_WIDTH - PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
	break;
	
	case 2:
		display_clearRect((PSD_DISPLAY_WIDTH + PSF_CHAR_WIDTH) / 2, 16, PSF_CHAR_WIDTH, PSF_CHAR_HEIGHT);
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
				setState(STATE_MENU);
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

void showPage() {
	display_clear();
	
	int size, i, offset;
	char * str = intToStr(PS_SCORE_PAGE_COUNT, &size);
	offset = size + 8;
	str = intToStr(vars.score.page + 1, &size);
	offset+= size;
	
	offset = (PSD_DISPLAY_WIDTH - (offset * PSF_CHAR_WIDTH)) / 2;
	
	display_putString(offset, 0, "Page (");
	display_putString(offset + 6 * PSF_CHAR_WIDTH, 0, str);
	
	display_putChar(offset + PSF_CHAR_WIDTH * (size + 6), 0, '/');
	str = intToStr(PS_SCORE_PAGE_COUNT, &i);
	display_putString(offset + PSF_CHAR_WIDTH * (size + 7), 0, str);
	display_putChar(offset + PSF_CHAR_WIDTH * (size + 7 + i), 0, ')');
	
	for (i = 0; i < PS_SCORE_PER_PAGE; i++) {
		display_putString(0, PSF_CHAR_HEIGHT * (i + 1), score_names[(vars.score.page * PS_SCORE_PER_PAGE) + i]);
		str = intToStr(score_scores[(vars.score.page * PS_SCORE_PER_PAGE) + i], &size);
		display_putString(PSD_DISPLAY_WIDTH - 1 - (size * PSF_CHAR_WIDTH), PSF_CHAR_HEIGHT * (i + 1), str);
	}
	display_show();
	return;
}

void loop_score() {
	unsigned char buttons = input_getButtons();
	
	if (buttons)
		usedButtons = buttons;
	
	if (lastTime == time_tick)
		return;
	
	lastTime = time_tick;
	
	if (buttons && ((time_tick - vars.score.lastSelect) >= PS_INPUT_PAD)) {
		vars.score.lastSelect = time_tick;
		if (usedButtons & 8) {
			if (vars.score.page == 0)
				vars.score.page = PS_SCORE_PAGE_COUNT - 1;
			else
				vars.score.page--;
			
			showPage();
		} else if (usedButtons & 1) {
			if (vars.score.page == PS_SCORE_PAGE_COUNT - 1)
				vars.score.page = 0;
			else
				vars.score.page++;
			
			showPage();
		} else {
			// Return
			setState(STATE_MENU);
		}
	}
	
	if ((time_tick - vars.score.lastSelect) > PS_IDLE_TICK) {
		vars.idle.lastState = STATE_SCORE;
		setState(STATE_IDLE);
	}
}

void loop_idle() {
	unsigned char buttons = input_getButtons();
	
	if (lastTime == time_tick)
		return;
	
	if (buttons) {
		display_setBrightness(0x7F);
		setState(vars.idle.lastState);
		return;
	}
	
	if (++vars.idle.x == PSD_DISPLAY_WIDTH) {
		vars.idle.x = 0;
		display_invert();
		PORTE = rand_next() % 256;
	}
	
	display_showRect(vars.idle.x, 0, 1, PSD_DISPLAY_HEIGHT);
	
	lastTime = time_tick;
}

void setState(STATE s) {
	PORTE = 0;
	switch (s) {
	case STATE_GAME:
		game_start(PS_SNAKE_SIZE, input_getSwitches());
		loop = &loop_game;
	break;
	
	case STATE_OVER:
		vars.over.lastSelect = time_tick;
		vars.over.substate = 0;
		int i;
		loop = &loop_over;
	break;
	
	case STATE_HIGH:
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
	
	case STATE_SCORE:
		vars.score.page = 0;
		showPage();
		vars.score.lastSelect = time_tick;
		loop = &loop_score;
	break;
	
	case STATE_IDLE:
		vars.idle.x = PSD_DISPLAY_WIDTH - 1;
		display_setBrightness(0);
		loop = &loop_idle;
	break;
	
	default:	// STATE_MENU
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