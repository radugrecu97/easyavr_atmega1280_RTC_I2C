/*
 * _keypad4x4_driver.c
 *
 * Created: 17-May-19 14:44:24
 *  Author: radug
 */ 

#include <avr/io.h>

void init_keyboard()
{
	//Enables pull-ups on Columns
	PORTJ |= 0b00001111;
}



int8_t getCh()
{
	uint8_t row, col;
	DDRJ = 0;
	for(row=0;row<4;row++)
	{
		//Set the (4-c) row to high
		DDRJ =(0b10000000>>row);
		
		for(col=0;col<4;col++)
		{
			//Determine which column pin is pulled LOW with a bitwise mask
			if(!(PINJ & (0b00001000>>col)))
			{
				return (char) (row*4+col);
			}
		}
	}

	return -1;
}