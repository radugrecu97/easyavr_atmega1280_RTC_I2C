/*
 * DS1337_driver.c
 *  Author: radug
 */ 

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "i2c_driver.h"
#include <avr/common.h>
#include <stdlib.h>

static uint8_t busy;
static uint8_t alarm_status;

#ifndef SLAVE_ADDRESS
#define SLAVE_ADDRESS 0b1101000
#endif

uint8_t RTC_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	// Hour
	uint8_t hour_value = ((hours / 10) << 4) + (hours % 10);
	// set to 24-hr mode
	hour_value &= 0b10111111;
	i2c_write_addr(SLAVE_ADDRESS, 0x02, &hour_value, 1);
	
	// Don't continue until transmission has finished
	while (i2c_status());
	
	// Minutes
	uint8_t min_value = ((minutes / 10) << 4) + (minutes % 10);
	
	i2c_write_addr(SLAVE_ADDRESS, 0x01, &min_value, 1);
	
	// Don't continue until transmission has finished
	while (i2c_status());
	
	// Seconds
	uint8_t sec_value = ((seconds / 10) << 4) + (seconds % 10);
	
	i2c_write_addr(SLAVE_ADDRESS, 0x00, &sec_value, 1);
	
	// Don't continue until transmission has finished
	while (i2c_status());
}

uint8_t RTC_set_date(uint8_t year, uint8_t month, uint8_t day)
{
	// Year
	// set tens
	uint8_t year_value = ((year / 10) << 4) + (year % 10);
	i2c_write_addr(SLAVE_ADDRESS, 0x06, &year_value, 1);
		
	//Don't continue until transmission has finished
	while (i2c_status());	
		
	//Month
	uint8_t month_value = ((month / 10) << 4) + (month % 10);
	
	// set century
	month_value |= (year/100) << 7;
	i2c_write_addr(SLAVE_ADDRESS, 0x05, &month_value, 1);
		
	//Don't continue until transmission has finished
	while (i2c_status());
		
	//Day
	uint8_t day_value = ((day / 10) << 4) + (day % 10);
		
	i2c_write_addr(SLAVE_ADDRESS, 0x04, &day_value, 1);
		
	//Don't continue until transmission has finished
	while (i2c_status());
}

uint8_t RTC_read_time(uint8_t *hour_buffer, uint8_t *min_buffer, uint8_t *sec_buffer)
{
	uint8_t *buffer = malloc(sizeof(uint8_t) * 3);

	//Hour
	i2c_read_addr(SLAVE_ADDRESS, 0x02, &buffer[0], 1);

	//Don't continue until transmission has finished
	while (i2c_status());
	*hour_buffer = ((buffer[0] >> 4) * 10) + (buffer[0] & 0x0f);
		
	//Minutes
	i2c_read_addr(SLAVE_ADDRESS, 0x01, &buffer[1], 1);
	
	//Don't continue until transmission has finished
	while (i2c_status());
	*min_buffer = ((buffer[1] >> 4) * 10) + (buffer[1] & 0x0f);

	//Seconds
	i2c_read_addr(SLAVE_ADDRESS, 0x00, &buffer[2], 1);
		
	//Don't continue until transmission has finished
	while (i2c_status());
	*sec_buffer = ((buffer[2] >> 4) * 10) + (buffer[2] & 0x0f);
	
	free(buffer);
}

uint8_t RTC_read_date(uint8_t *year_buffer, uint8_t *month_buffer, uint8_t *day_buffer)
{
	uint8_t *buffer = malloc(sizeof(uint8_t) * 3);

	// Year
	// read tens
	i2c_read_addr(SLAVE_ADDRESS, 0x06, &buffer[0], 1);

	//Don't continue until transmission has finished
	while (i2c_status());
	*year_buffer = ((buffer[0] >> 4) * 10) + (buffer[0] & 0x0f);
		
	//Month
	i2c_read_addr(SLAVE_ADDRESS, 0x05, &buffer[1], 1);
		
	// Don't continue until transmission has finished
	while (i2c_status());
	*month_buffer = (((buffer[1] & 0b00010000) >> 4) * 10) + (buffer[1] & 0x0f);
	// Add century
	*year_buffer += (buffer[1] & 0b10000000) * 100;
	//Day
	i2c_read_addr(SLAVE_ADDRESS, 0x04, &buffer[2], 1);
		
	//Don't continue until transmission has finished
	while (i2c_status());
	*day_buffer = ((buffer[2] >> 4) * 10) + (buffer[2] & 0x0f);
	
	free(buffer);
}


uint8_t RTC_set_alarm(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t dy_dt)
{
	alarm_status = 0;
	
	// Enable interrupts
	uint8_t INTCN = 0b00000101;
	
	i2c_write_addr(SLAVE_ADDRESS, 0x0E, &INTCN, 1);
	
	// Don't continue until transmission has finished
	while (i2c_status());
	
	// Hour
	uint8_t hour_value = ((hours / 10) << 4) + (hours % 10);
	// set to 24-hr mode & clear alarm mask bit
	hour_value &= 0b10111111;
	hour_value &= 0b00111111;
	i2c_write_addr(SLAVE_ADDRESS, 0x09, &hour_value, 1);
		
	// Don't continue until transmission has finished
	while (i2c_status());
		
	// Minutes
	uint8_t min_value = ((minutes / 10) << 4) + (minutes % 10);
	// clear alarm mask bit
	min_value &= 0b01111111;
	i2c_write_addr(SLAVE_ADDRESS, 0x08, &min_value, 1);
		
	// Don't continue until transmission has finished
	while (i2c_status());
		
	// Seconds
	uint8_t sec_value = ((seconds / 10) << 4) + (seconds % 10);
	// clear alarm mask bit
	sec_value &= 0b01111111;
	i2c_write_addr(SLAVE_ADDRESS, 0x07, &sec_value, 1);
		
	// Don't continue until transmission has finished
	while (i2c_status());
	
 	uint8_t day_value = ((day / 10) << 4) + (day % 10);
 	
 	day_value ^= dy_dt << 6;
	day_value |= 0b10000000;
 	i2c_write_addr(SLAVE_ADDRESS, 0x0A, &day_value, 1);
 	
	//Don't continue until transmission has finished
 	while (i2c_status());
}

/**
 * Return 1 if alarm was triggered
 */

uint8_t get_alarm_status()
{
	return alarm_status;
}

void clear_alarm_status()
{
	alarm_status = 0;
}

// Alarm
ISR(INT2_vect)
{
	alarm_status = 1;
	EICRA |= _BV(ISC21);
	EIMSK |= _BV(INT2);
}