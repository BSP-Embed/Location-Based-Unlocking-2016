//INCLUDE KEYBOARD HEADER FILE

#include"mkb.h"


//DECLARE GLOABL VARIABLE

unsigned char get_data(void)
{
	unsigned char  time_out = 0;
	unsigned char digit = 0;

	char a[] = {'1','2','3',
				'4','5','6',
				'7','8','9',
				'*','0','#' };	

	
	KEYBOARD_DDR = 0xf0;
	KEYBOARD_PORT = 0x0f;
	
	while((PINA  & 0x0f) != 0x0f);
	dlyms(1);
	while((PINA  & 0x0f) != 0x0f);
	while((PINA  & 0x0f) == 0x0f);
	
	dlyms(1);
	KEYBOARD_PORT = (KEYBOARD_PORT & 0X0F) | 0xe0;
	dlyms(1);
  	if( (PINA & 0x0f) != 0x0f)
   		digit = find_col(0x00);
	else
	{
		KEYBOARD_PORT = (KEYBOARD_PORT & 0X0F) | 0xD0;
		dlyms(1);
		if((PINA & 0x0f) != 0x0f)
			digit = find_col(0x03); 
		else
		{
			KEYBOARD_PORT = (KEYBOARD_PORT & 0X0F) | 0xB0;
			dlyms(1);
			if( (PINA & 0x0f)  != 0x0f)
				digit = find_col(0x06);	 
			else
			{
				KEYBOARD_PORT = (KEYBOARD_PORT & 0X0F) | 0x70;
				dlyms(1);
				if( (PINA & 0x0f)  != 0x0f)
					digit = find_col(0x09);
			}
		}
	}
	return a[digit];
}
	
unsigned char find_col(unsigned char key)
{ 

	if((PINA & 0x0e) == 0x0a)
		key++;
	else if((PINA & 0x0e) == 0x06)
		key += 2;
	return key;
}

