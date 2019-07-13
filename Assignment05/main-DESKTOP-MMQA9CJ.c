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

#ifndef F_CPU
	#define F_CPU 7372800L
#endif

int main(void)
{
    /* Replace with your application code */

	uint8_t hours = 0,
			minutes = 0,
			seconds = 0,
			year = 0,
			month = 0,
			day = 0,
			lastPrinted = 0;
				
	i2c_init(50000);
	init_7_segment();
	sei();

	hours = 2;
	minutes = 3;
	seconds = 5;
	
	year = 19;
	month = 5;
	day = 2;
	
	RTC_set_time(hours, minutes, seconds);
	RTC_set_date(year, month, day);
	printint_7_segment(0);
	
    while (1) 
    {
		//RTC_read_date(&year, &month, &day);
		
		RTC_read_time(&hours, &minutes, &seconds);
		
		if (lastPrinted != seconds)
		{
			lastPrinted = seconds;
			//printint_7_segment(month*100 + day);
			printint_7_segment(minutes*100 + seconds);
		}

		_delay_ms(800);
    }
}

