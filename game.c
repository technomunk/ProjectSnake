#include "game.h"

unsigned int game_field[PSG_FIELD_SIZE];

unsigned int game_score, game_score_multiplier;

int head_x, head_y, tail_x, tail_y, oneUp_x, oneUp_y;
unsigned char oneUp, lastWalls;
direction invDir;

void genOneUp() {
	oneUp_x = 1 + rand_next() % (PSD_DISPLAY_WIDTH - 2);
	oneUp_y = 1 + rand_next() % (PSD_DISPLAY_HEIGHT - 2);
	
	if (game_getField(oneUp_x, oneUp_y) == PSG_FIELD_EMPTY) {
		oneUp = 2;
		game_setField(oneUp_x, oneUp_y, PSG_FIELD_ONEUP);
	} else
		oneUp = 0;
}

void game_blinkOneUp() {
	if (!oneUp)
		return;
	
	oneUp = (oneUp & 1) ? 2 : 3;
	display_update(oneUp_x, oneUp_y, oneUp & 1);
}

// Does not check collision, to optimize game initialization
void setWalls(unsigned char walls) {
	int i;
	
	if (walls & 8)
		for (i = 0; i < PSD_DISPLAY_HEIGHT; i++) {
			game_setField(0, i, PSG_FIELD_WALL);
			game_setField(PSD_DISPLAY_WIDTH - 1, i, PSG_FIELD_WALL);
			display_put(0, i, 1);
			display_put(PSD_DISPLAY_WIDTH - 1, i, 1);
		}
		
		
	if (walls & 4)
		for (i = 0; i < PSD_DISPLAY_WIDTH / 3 + 1; i++) {
			game_setField(i, 0, PSG_FIELD_WALL);
			game_setField(i, PSD_DISPLAY_HEIGHT - 1, PSG_FIELD_WALL);
			display_put(i, 0, 1);
			display_put(i, PSD_DISPLAY_HEIGHT - 1, 1);
		}
		
	
	if (walls & 2)
		for (i = PSD_DISPLAY_WIDTH / 3 + 1; i < PSD_DISPLAY_WIDTH / 3 * 2 + 1; i++) {
			game_setField(i, 0, PSG_FIELD_WALL);
			game_setField(i, PSD_DISPLAY_HEIGHT - 1, PSG_FIELD_WALL);
			display_put(i, 0, 1);
			display_put(i, PSD_DISPLAY_HEIGHT - 1, 1);
		}
		
		
	if (walls & 1)
		for (i = PSD_DISPLAY_WIDTH / 3 * 2 + 1; i < PSD_DISPLAY_WIDTH; i++) {
			game_setField(i, 0, PSG_FIELD_WALL);
			game_setField(i, PSD_DISPLAY_HEIGHT - 1, PSG_FIELD_WALL);
			display_put(i, 0, 1);
			display_put(i, PSD_DISPLAY_HEIGHT - 1, 1);
		}
}


field game_getField(int x, int y) {

#if PSG_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
#endif	// PSG_VALIDATE_ARGS
	
	return ((game_field[(y * PSG_FIELD_WIDTH) + x / PSG_PACK_RATE] >> ((x % PSG_PACK_RATE) * 3)) & 0x7);
}

void game_setField(int x, int y, field f) {
	
#if PSG_VALIDATE_ARGS
	x %= PSD_DISPLAY_WIDTH;
	y %= PSD_DISPLAY_HEIGHT;
	f &= 3;
#endif	// PSG_VALIDATE_ARGS
	
	// Remember field to avoid calculating it twice
	field * pf = &(game_field[(y * PSG_FIELD_WIDTH) + x / PSG_PACK_RATE]);
	field tmp = *pf;
	tmp &= ~(0x7 << ((x % PSG_PACK_RATE) * 3));
	tmp |= f << ((x % PSG_PACK_RATE) * 3);
	*pf = tmp;
}

void game_start(unsigned char size, unsigned char walls) {
	
#if PSG_VALIDATE_ARGS
	size %= PSD_DISPLAY_HEIGHT;
#endif	// PSG_VALIDATE_ARGS

	int i;
	
	display_clear();
	
	// Reset game field
	for (i = 0; i < PSG_FIELD_SIZE; i++)
		game_field[i] = 0;
	
	setWalls(walls);
	lastWalls = walls;
	
	int start = (PSD_DISPLAY_HEIGHT - size) / 2;
	
	for (i = 0; i < size; i++) {
		game_setField(PSD_DISPLAY_WIDTH / 2, start + i, PSG_FIELD_SNAKE_UP);
		display_put(PSD_DISPLAY_WIDTH / 2, start + i, 1);
	}
	
	head_x = PSD_DISPLAY_WIDTH / 2;
	head_y = start;
	
	tail_x = PSD_DISPLAY_WIDTH / 2;
	tail_y = start + i - 1;
	
	game_score = 0;
	
	rand_seed(time_tick);
	oneUp = 0;
	while (!oneUp)
		genOneUp();	// as genOneUp() only tries to generate, make sure we get something
	
	display_put(oneUp_x, oneUp_y, 1);
	display_show();
	
	invDir = PSG_DIR_DOWN;
}

int game_update(direction dir) {
	
	// Make sure snake can't turn 180
	if (dir == invDir)
		switch (dir) {
		case PSG_DIR_RIGHT:
			dir = PSG_DIR_LEFT;
			break;
		case PSG_DIR_DOWN:
			dir = PSG_DIR_UP;
			break;
		case PSG_DIR_LEFT:
			dir = PSG_DIR_RIGHT;
			break;
		default:
			dir = PSG_DIR_DOWN;
			break;
		}
		
	switch (dir) {
		
	case PSG_DIR_RIGHT:
		game_setField(head_x, head_y, PSG_FIELD_SNAKE_RIGHT);
		if (++head_x == PSD_DISPLAY_WIDTH)
			head_x = 0;
		
		invDir = PSG_DIR_LEFT;
	break;
	
	case PSG_DIR_DOWN:
		game_setField(head_x, head_y, PSG_FIELD_SNAKE_DOWN);
		if (++head_y == PSD_DISPLAY_HEIGHT)
			head_y = 0;
		
		invDir = PSG_DIR_UP;
	break;
	
	case PSG_DIR_LEFT:
		game_setField(head_x, head_y, PSG_FIELD_SNAKE_LEFT);
		if (--head_x == -1)
			head_x += PSD_DISPLAY_WIDTH;
		
		invDir = PSG_DIR_RIGHT;
	break;
	
	// (PSD_DIR_UP) will go here
	default:
		game_setField(head_x, head_y, PSG_FIELD_SNAKE_UP);
		if (--head_y == -1)
			head_y += PSD_DISPLAY_HEIGHT;
		
		invDir = PSG_DIR_DOWN;
	break;
	}
	
	field head = game_getField(head_x, head_y);
	if (PSG_FIELD_KILLS(head))
		return 0;
	
	game_setField(head_x, head_y, PSG_FIELD_SNAKE_UP);
	display_update(head_x, head_y, 1);
	
	if (head == PSG_FIELD_ONEUP) {
		game_score += game_score_multiplier;
		oneUp = 0;
		goto skipTail;
	}
	
	field tail = game_getField(tail_x, tail_y);
	game_setField(tail_x, tail_y, PSG_FIELD_EMPTY);
	display_update(tail_x, tail_y, 0);
	
	switch (tail) {
		case PSG_FIELD_SNAKE_RIGHT:
			if (++tail_x == PSD_DISPLAY_WIDTH)
				tail_x = 0;
		break;
		
		case PSG_FIELD_SNAKE_DOWN:
			if (++tail_y == PSD_DISPLAY_HEIGHT)
				tail_y = 0;
		break;
		
		case PSG_FIELD_SNAKE_LEFT:
			if (--tail_x == -1)
				tail_x += PSD_DISPLAY_WIDTH;
		break;
		
		default:	// PSG_FIELD_SNAKE_UP
			if (--tail_y == -1)
				tail_y += PSD_DISPLAY_HEIGHT;
		break;
	}
	
skipTail:
	if (!oneUp)
		genOneUp();
	return 1;
}

int game_updateWalls(unsigned char id) {
	if (lastWalls == id)
		return 1;
	
	int i, check, hit = 1;
	if ((lastWalls & 8) != (id & 8)) {
		check = id & 8;
		for (i = 0; i < PSD_DISPLAY_HEIGHT; i++) {
			if (PSG_FIELD_IS_SNAKE(game_getField(0, i)) || PSG_FIELD_IS_SNAKE(game_getField(PSD_DISPLAY_WIDTH - 1, i)))
				if (check)
					hit = 0;
			game_setField(0, i, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			game_setField(PSD_DISPLAY_WIDTH - 1, i, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			display_put(0, i, check);
			display_put(PSD_DISPLAY_WIDTH - 1, i, check);
		}
	}

	if ((lastWalls & 4) != (id & 4)) {
		check = id & 4;
		for (i = 0; i < PSD_DISPLAY_WIDTH / 3 + 1; i++) {
			if (PSG_FIELD_IS_SNAKE(game_getField(i, 0)) || PSG_FIELD_IS_SNAKE(game_getField(i, PSD_DISPLAY_HEIGHT - 1)))
				if (check)
					hit = 0;
			game_setField(i, 0, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			game_setField(i, PSD_DISPLAY_HEIGHT - 1, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			display_put(i, 0, check);
			display_put(i, PSD_DISPLAY_HEIGHT - 1, check);
			
			skip4:;
		}
	}
	
	if ((lastWalls & 2) != (id & 2)) {
		check = id & 2;
		for (i = PSD_DISPLAY_WIDTH / 3 + 1; i < PSD_DISPLAY_WIDTH / 3 * 2 + 1; i++) {
			if (PSG_FIELD_IS_SNAKE(game_getField(i, 0)) || PSG_FIELD_IS_SNAKE(game_getField(i, PSD_DISPLAY_HEIGHT - 1)))
				if (check)
					hit = 0;
			game_setField(i, 0, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			game_setField(i, PSD_DISPLAY_HEIGHT - 1, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			display_put(i, 0, check);
			display_put(i, PSD_DISPLAY_HEIGHT - 1, check);
		}
	}
	
	if ((lastWalls & 1) != (id & 1)) {
		check = id & 1;
		for (i = PSD_DISPLAY_WIDTH / 3 * 2 + 1; i < PSD_DISPLAY_WIDTH; i++) {
			if (PSG_FIELD_IS_SNAKE(game_getField(i, 0)) || PSG_FIELD_IS_SNAKE(game_getField(i, PSD_DISPLAY_HEIGHT - 1)))
				if (check)
					hit = 0;
			game_setField(i, 0, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			game_setField(i, PSD_DISPLAY_HEIGHT - 1, (check) ? PSG_FIELD_WALL : PSG_FIELD_EMPTY);
			display_put(i, 0, check);
			display_put(i, PSD_DISPLAY_HEIGHT - 1, check);
		}
	}
		
	display_show();
	lastWalls = id;
	
	return hit;
}