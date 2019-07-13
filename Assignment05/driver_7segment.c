/*
 * driver_7segment.c
 *
 * Created: 06-Apr-19 16:26:11
 *  Author: radug
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/**
 * Variable declaration
 */

uint8_t *valueHex,
numberLength,
currentSegment;

/**
 * Function declaration
 */

uint8_t digitToHex(uint8_t digit);
void init_timer0();

void init_7_segment()
{
	//set SHCP(DDB1), DS(DDB2) and MR(DDB) as output
	DDRB |= _BV(DDB1) | _BV(DDB2) | _BV(DDB3);
	
	//set STCP as output
	DDRK |= _BV(DDK3);
	
	//set MR to high as it is active low
	PORTB |= _BV(PB3);
	
	//set as output
	DDRL = 0x0F;
}

/**
 * Compare register value was calculated using the forumla: OCR = (7372800 - (refresh rate * 2 * pre-scaler))/(refreshrate * 2 * pre-scaler)
 */

void init_timer0()
{
	//reset to avoid incorrect setup
	TCCR4A = 0;
	TCCR4B = 0;
 
	//initialize compare value
	//f = 360 hz
	//lower refresh rates are suitable as long as there are less than 4 segments lit up at the same time
	OCR4A=159;
	
	//reset counter
	TCNT4=0;
 
	//pre-scaler to clk/64
	TCCR4B |= _BV(CS42);
	TCCR4B &= ~(_BV(CS41));
	TCCR4B &= ~(_BV(CS40));
	
	//CTC mode
	TCCR4B |= _BV(WGM42);

	//enable compare interrupt
	TIMSK4 |= _BV(OCIE4A);
}

/**
 * Printing function for integers
 */

void printint_7_segment(uint16_t value)
{
	currentSegment = 0;
	numberLength = 0;
	free(valueHex);
	uint16_t temp = value;
	
	// get number of digits
	for (int i = 0; i<4; i++)
	{
		if ((temp /= 10) < 0)
			break;
		numberLength++;
	}
		
	valueHex = malloc(sizeof(uint8_t) * numberLength);
	
	// convert each of the value's digit to HEX
	for (int i = 0; i<4; i++)
	{
		if (value < 0)
			break;
		valueHex[i] = digitToHex(value % 10);
		value /= 10;
	}
	
	init_timer0();
}

/**
 * Printing function for decimals
 */

void display_7_segment(float value, uint8_t no_of_decimals)
{
	uint16_t valueInt;

	//convert float to int
	for (int i = 0; i<no_of_decimals; i++)
	{
		value = value * 10;
	}
	
	//0.5 is added to the value because the value may not be represented exactly in binary
	valueInt = (int) (value + 0.5);
	
	printint_7_segment(valueInt);

	//set dp section to 1 if at least 1 decimal
	if (no_of_decimals > 0)
		valueHex[no_of_decimals]++;
		
	init_timer0();
}

/**
 * For each digit to be represented, specific segments need to be lit up.
 */

uint8_t digitToHex(uint8_t digit)
{
	uint8_t hexValue;
	switch (digit)
	{
		case 0: hexValue = 0xFC;
		break;
		case 1: hexValue = 0x60;
		break;
		case 2: hexValue = 0xDA;
		break;
		case 3: hexValue = 0xF2;
		break;
		case 4: hexValue = 0x66;
		break;
		case 5: hexValue = 0xB6;
		break;
		case 6: hexValue = 0xBE;
		break;
		case 7: hexValue = 0xE0;
		break;
		case 8: hexValue = 0xFE;
		break;
		case 9: hexValue = 0xF6;
		break;
		//set to 0 if undefined
		default: hexValue = 0xFC;
		break;
	}
	return hexValue;	
}




ISR(TIMER4_COMPA_vect)
{
	//push to storage register
	for (int j = 0; j<8; j++)
	{
		//load MSB firsts
		if ((valueHex[currentSegment] >> j) & 1)
			PORTB |= _BV(PB2);
		else
			PORTB &= ~(_BV(PB2));
		
		//set SHCP to high
		PORTB |= _BV(PB1);
		
		//set SHCP to low
		PORTB &= ~(_BV(PB1));
	}

	//clear all the displaying segments then light-up the segment on the current position
	PORTL &= ~(_BV(PL0) | _BV(PL1) | _BV(PL2) | _BV(PL3));
	
	//select which segment the byte will be displayed on
	PORTL |= _BV(currentSegment);
	
	//shift byte to display
	//set latch high
	PORTK |= _BV(PK3);
	
	//set latch low
	PORTK &= ~(_BV(PK3));
	
	//move to next segment
	currentSegment++;
	
	//reset segment position once the last digit is printed
	if (currentSegment == numberLength)
		currentSegment = 0;
}