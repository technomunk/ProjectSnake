/*
	A high level time and timer related library
*/

#ifndef _PS_TIME
#define _PS_TIME

#include <pic32mx.h>

// This will depend on the clock in question, for Uno32 board the clock is 80MHz
#define PST_SINGLE_MS 80000
// Feel free to redifine this
// to calculate ticks per second divede 1000 by this number
#define PST_TICK_PERIOD_MS 25	// we use 25 to cap the frame rate at 40

extern unsigned int time_tick;

/*
	Handles interrupts
	This function will need to be moved elsewhere if you want to expand amount of handled interrupts
	For the purpose of the game we will use polling for other events
*/
void interrupt_handler();

/*
	Sets up all necessary ports for the timer
*/
void time_initialize();

/*
	Haults the operation until the time has passed
	IE returns only when the given amount of time has passed
	
	Arguments:
		unsigned int	- time in ms to wait
*/
void time_wait(unsigned int ms);

#endif // !_PS_TIME