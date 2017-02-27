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

/*
	Translates an integer to null-terminated string (radix 10)
	
	Arguments:
		unsigned int	- the integer value to translate (unsigned)
		int	*			- pointer to an integer to store the size of the string (or 0 to ignore)
		
	Returns:
		char *			- pointer to the first char of the string
*/
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

/*
	Seed the RNG
	
	Arguments:
		unsigned int	- seed
*/
void rand_seed(unsigned int seed);

/*
	Retreive the next pseudo-random value of the RNG
	
	Returns:
		int		- pseudo-random integer
*/
int rand_next();

/*
	Copies the source string to destination
	
	Arguments:
		char *		- pointer to the destination string
		char *		- pointer to the source null-terminated string
*/
void strcpy(char * destination, char * source);

#endif // !_PS_UTILITY