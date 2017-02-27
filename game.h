/*
	Functions that drive the game logic
*/

#ifndef _PS_GAME
#define _PS_GAME

#include "display.h"

#define PSG_VALIDATE_ARGS		0

#define PSG_DIR_UP				0
#define PSG_DIR_RIGHT			2
#define PSG_DIR_DOWN			4
#define PSG_DIR_LEFT			6


#define PSG_FIELD_SNAKE_UP		(1 | PSG_DIR_UP)
#define PSG_FIELD_SNAKE_RIGHT	(1 | PSG_DIR_RIGHT)
#define PSG_FIELD_SNAKE_DOWN	(1 | PSG_DIR_DOWN)
#define PSG_FIELD_SNAKE_LEFT	(1 | PSG_DIR_LEFT)


#define PSG_FIELD_EMPTY			0
#define PSG_FIELD_WALL			2
#define PSG_FIELD_ONEUP			4

#define PSG_FIELD_KILLS(x)		(x & 0x3)
#define PSG_FIELD_IS_SNAKE(x)	(x & 1)


// We pack 10 fields into a single int to save memory
#define PSG_PACK_RATE			10
#define PSG_FIELD_WIDTH			(PSD_DISPLAY_WIDTH / PSG_PACK_RATE + 1)
#define PSG_FIELD_HEIGHT		PSD_DISPLAY_HEIGHT
#define PSG_FIELD_SIZE			PSG_FIELD_WIDTH * PSG_FIELD_HEIGHT

// We are only interested in 3 least significant bits actually...
typedef unsigned int field;
typedef unsigned char direction;

extern int leds;

void game_show();

// Avoid adressing directly, as this is a packed array
extern unsigned int game_field[PSG_FIELD_SIZE];

// Stores current score, read this after game_udpate() returns 0 but before calling game_start() to read the final score
extern unsigned int game_score;
// Stores current score multiplier, change this with different speeds and settings
extern unsigned int game_score_multiplier;

/*
	Returns field at given coordinates
*/
field game_getField(int x, int y);

/*
	Sets the field to given value
	Also updates the screen
*/
void game_setField(int x, int y, field f);

void game_blinkOneUp();

/*
	Resets the game, starts with a snake of given size
	Also sets up the screen
	walls should come from switches
*/
void game_start(unsigned char size, unsigned char walls);

/*
	Updates the game one step
	Also updates the screen
	
	arguments:
		direction
	
	returns:
		0 - if the game is over
		1 - otherwise
*/
int game_update(direction dir);

/*
	Will only update changed walls
	returns:
		0 - if walls spawned on snake (game over)
		1 - otherwise
*/
int game_updateWalls(unsigned char walls);

#endif	// !_PS_GAME