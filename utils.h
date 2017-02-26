/*
	A helper utility library
*/

#ifndef _PS_UTILITY
#define _PS_UTILITY

#include <pic32mx.h>
#include <stdint.h>

typedef uint8_t byte;

// For use with intToStr()
extern char intStr[];

char * intToStr(unsigned int translate, int * pSize);

/*
	Sets up the required ports and configures the connection
*/
void spi_initialize();

/*
	Sends/recieves a single byte of data
	
	Arguments:
		byte	- the data to be sentry
	
	Returns:
		byte 	- recieved byte of data
*/
byte spi_byte(byte data);

/*
	Sends an array of data
	Recieves one byte of data/ depreciated for recieving
	
	Arguments:
		byte*		 	- pointer to first element of the array to be sentry
		unsigned int	- size of the array to be sent
		
	Returns
		byte			- last recieved byte
*/
byte spi_array(byte * pData, unsigned int dataSize);

void rand_seed(unsigned int seed);
int rand_next();

void strcpy(char * destination, char * source);

#endif // !_PS_UTILITY