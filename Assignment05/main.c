/*
 * Assignment05.c
 *
 * Created: 10-May-19 10:50:27
 * Author : radug
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "driver_7segment.h"
#include "i2c_driver.h"
#include "DS1337_driver.h"
#include "keypad4x4_driver.h"
#include <stdbool.h>

#define FORMAT_MM_SS 0
#define FORMAT_HH_MM 1
#define FORMAT_MM_DD 2
#define FORMAT_YYYY 3


#define KEY_FORMAT 0
#define KEY_EDIT 1
#define KEY_INCREMENT 2
#define KEY_DECREMENT 3
#define KEY_SWITCH 4
#define KEY_ALARM 5

#ifndef F_CPU
#define F_CPU 7372800L
#endif

#ifndef SLAVE_ADDRESS
#define SLAVE_ADDRESS 0b1101000
#endif

uint8_t hours,
		minutes,
		seconds,
		year,
		month,
		day,
		temp;

uint16_t currentReading,
		 lastPrinted;

uint8_t lastPressed, 
		currentKey, 
		format, 
		*selected;
		
bool editMode;

void switch_selection();

int main(void)
{
	//Driver initialization
	i2c_init(50000);
	init_7_segment();
	init_keyboard();

/*	
	DDRD &= ~(_BV(DDD2));

	EICRA |= _BV(ISC21);
	EIMSK |= _BV(INT2);
*/	

	lastPressed = 255;
	lastPrinted = 0;
	currentKey  = 0;
	format = 0;
	currentReading = 0;
    editMode = false;

	selected = malloc(sizeof(uint8_t));
	
	hours = 23;
	minutes = 32;
	seconds = 50;
	
	year = 19;
	month = 11;
	day = 27;
	temp = 0;
	uint8_t buffer;

	sei();

	//RTC_set_alarm(hours, minutes, seconds+5, day, 0);
	RTC_set_time(hours, minutes, seconds);
	RTC_set_date(year, month, day);
	
	
	
    while (1) 
    {
		
		//Alarm
		/*	
		if (get_alarm_status())
		{
			
			// Clear interrupt trigger
			i2c_read_addr(SLAVE_ADDRESS, 0x0F, &buffer, 1);
	
			// Don't continue until transmission has finished
			while (i2c_status());
			
			i2c_read_addr(SLAVE_ADDRESS, 0x0E, &buffer, 1);
	
			// Don't continue until transmission has finished
			while (i2c_status());
			
			
			// Clear interrupt trigger
			i2c_write_addr(SLAVE_ADDRESS, 0x0F, &temp, 1);
	
			// Don't continue until transmission has finished
			while (i2c_status());
			

			
			clear_alarm_status();
			
			printint_7_segment(7777);
			_delay_ms(10000);
			RTC_set_alarm(hours, minutes, seconds+5, day, 0);
			
			i2c_read_addr(SLAVE_ADDRESS, 0x0E, &buffer, 1);
	
			// Don't continue until transmission has finished
			while (i2c_status());
		}
		*/
		
		// Display Format
		currentKey = getCh();
		if (lastPressed != currentKey)
		{
			lastPressed = currentKey;
			switch (lastPressed)
			{
				case KEY_FORMAT:
					if (format >= 3)
						format = 0;
					else
						format++;
					if (editMode)
						switch_selection();
					break;
					
				case KEY_EDIT:
					if (!editMode)
						editMode = true;
					else
					{
						RTC_set_time(hours, minutes, seconds);
						RTC_set_date(year, month, day);
						editMode = false;
					}
					break;
					
				case KEY_INCREMENT:
					(*selected)++;
					break;
					
				case KEY_DECREMENT:
					(*selected)--;
					break;
					
				case KEY_SWITCH:
					switch_selection();
					break;
					
				case KEY_ALARM:
					break;
					
				default:
					break;
			}
		}
		
				
		// Read time only when not in edit mode
		if (!editMode)
		{		
			RTC_read_date(&year, &month, &day);
			RTC_read_time(&hours, &minutes, &seconds);
		}
		
		switch (format)
		{
			case FORMAT_MM_SS:
				currentReading = minutes*100 + seconds;
				if (lastPrinted != currentReading)
				{
					lastPrinted = currentReading;
					printint_7_segment(lastPrinted);
				}
				break;
			
			case FORMAT_HH_MM:
				currentReading = hours*100 + minutes;
				if (lastPrinted != currentReading)
				{
					lastPrinted = currentReading;
					printint_7_segment(lastPrinted);
				}
				break;
			
			case FORMAT_MM_DD:
				currentReading = month*100 + day;
				if (lastPrinted != currentReading)
				{
					lastPrinted = currentReading;
					printint_7_segment(lastPrinted);
				}
				break;
			
			case FORMAT_YYYY:
				currentReading = year+2000;
				if (lastPrinted != currentReading)
				{
					lastPrinted = currentReading;
					printint_7_segment(lastPrinted);
				}
				break;
			
			default:
				break;
		}
    }
}

void switch_selection()
{
	switch (format)
	{
		case FORMAT_MM_SS:
			if (selected == &seconds)
				selected = &minutes;
			else
				selected = &seconds;
			break;
		
		case FORMAT_HH_MM:
			if (selected == &minutes)
				selected = &hours;
			else
				selected = &minutes;
			break;
		
		case FORMAT_MM_DD:
			if (selected == &day)
				selected = &month;
			else
				selected = &day;
			break;
		
		case FORMAT_YYYY:
			selected = &year;
			break;
		
		default:
			break;
	}
}