/*
	A high level time and timer related library
*/

#ifndef PS_TIME
#define PS_TIME

#include <pic32mx.h>

#define PST_SINGLE_MS 80000
// Feel free to redifine this to any non-zero multiple of 10
// to calculate "fps" divide 1000 by this number
#define PST_PERIOD_MS 20

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