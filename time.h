/*
	A high level time and timer related library
*/

#ifndef PS_TIME
#define PS_TIME

#include <pic32mx.h>

// This will depend on the clock in question, for Uno32 board the clock is 80MHz
#define PST_SINGLE_MS 80000
// Feel free to redifine this to any non-zero multiple of 2
// to calculate ticks per second divede 1000 by this number
#define PST_TICK_PERIOD_MS 20	// we use 20 to cap the frame rate at 50

extern unsigned int time_tick;

// Handles interrupts
// This function will need to be moved elsewher if you want to expant amount of handled interrupts
// For the purpose of the game we will use polling for other events
void interrupt_handler();

// Initializes all needed ports and stuff
void time_initialize();

// Haults the operation until the time has passed
void time_wait(unsigned int ms);

#endif // !PS_TIME