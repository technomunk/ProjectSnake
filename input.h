/*
	A library for reading inputs from the IO Shield
*/

#ifndef _PS_INPUT
#define _PS_INPUT

#include <pic32mx.h>

void input_initialize();

unsigned int input_readDial();

unsigned int input_readButtons();

unsigned int input_readSwitches();

#endif	// !_PS_INPUT