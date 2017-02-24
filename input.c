#include "input.h"

void input_initialize() {
	
	//	=======================
	//	=== Initialize dial ===
	//	=======================
	
	AD1PCFG = ~0x4;
	TRISBSET = 0x4;
	
	AD1CHS |= (2 << 16);
	
	AD1CON1 |= (4 << 8) | (7 << 5);
	AD1CON2 &= 0;
	AD1CON3	|= (1 << 15);
	
	// Turn on ADC
	AD1CON1 |= (1 << 15);
	
	// =======================================
	// === Initialize Buttons and Switches ===
	// =======================================
	
	TRISDSET = 0x7F0;	// Buttons 4-2 and Switches
	TRISFSET = 0x2;		// Button 1
}

unsigned int input_getDial() {
	AD1CON1 |= 2;
	while (!(AD1CON1 & 2));
	while (!(AD1CON1 & 1));
	
	return ADC1BUF0;
}

unsigned int input_getButtons() {
	return ((PORTD >> 4) & 0xE) | ((PORTF >> 1) & 1);
}

unsigned int input_getSwitches() {
	return (PORTD >> 8) & 0xF;
}