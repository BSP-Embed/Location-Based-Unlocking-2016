#ifndef KEYBOARD4_4_H
#define KEYBOARD4_4_H


#include "includes.h"

//DEFINE PORTS/PINS
#define KEYBOARD_DDR		DDRA
#define KEYBOARD_PORT		PORTA
#define key_data 			PINA

//DEFINE CONSTANT
#define KEY_DELAY 			100
#define MAX_TIME_OUT 		100
#define MASK 			(key_data & 0x0e)

//FUNCTION PROTOTYPES 
unsigned char get_data(void);
unsigned char  find_col(unsigned char key);

#endif
