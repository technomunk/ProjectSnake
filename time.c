/*
	Implementation of the time.h library
*/

#include "time.h"

unsigned int time_tick = 0;

/*
	A very basic get-in-get-out handler for the interrupts
	======================================================
	by
		Grigory Glukhov
*/
void interrupt_handler() {
	
	// Make sure we only handle timer based interrupt
	if (IFS(0) & 0x100) {
		IFSCLR(0) = 0x100;	// reset interrupt flag
		
		time_tick++;
	}
}

/*
	Get the TIMER2 ready for our use
	================================
	by
		Grigory Glukhov
*/
void time_initialize() {
	
	T2CONSET = 0;	// disable timer

	IFSCLR(0) = 0x100;		// reset interrupt flag
	IECSET(0) = 0x100;		// enable interrupt
	
	IPCSET(2) = 0x1F;		// set interrupt priority to max
	
	TMR2 = 0;									// reset timer
	PR2 = PST_SINGLE_MS * PST_PERIOD_MS / 256;	// set period

	T2CONSET = ((1 << 15)		// enable timer
				| (7 << 4));	// prescale 1:256
}

/*
	Haults the operation for at least the given amount of ms
	========================================================
	by
		Grigory Glukhov
*/
void time_wait(unsigned int ms) {
	
	if (ms % PST_PERIOD_MS)
		ms = ms / PST_PERIOD_MS + 1;
	else
		ms /= PST_PERIOD_MS;
	
	unsigned int lastTick = time_tick;
	
	while (time_tick - lastTick < ms);
	
	return;
}
