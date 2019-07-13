/*
 * i2c_driver.c
 *  Author: radug
 */ 

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "i2c_driver.h"
#include <avr/common.h>

/**
 * Definitions
 */

#define TWI_NOT_BUSY 0
#define TWI_BUSY 1
#define WRITING 0
#define READING 1

#define TWCR_DEFAULT (_BV(TWEA) | _BV(TWEN) | _BV(TWIE))

#define TWCR_NOT_ACK (_BV(TWINT) | _BV(TWEN) | _BV(TWIE))
#define TWCR_ACK (TWCR_NOT_ACK | _BV(TWEA))

/**
 * Variable declaration
 */

static uint8_t busy;

// Current action - R = 1; W = 0					 
static uint8_t slave_address,
		register_address,
		*wr_bytes,
		wr_bytes_count,
		current_action,
		*read_buffer,
		read_max,
		counter;


uint8_t i2c_init(long scl)
{
	// Set prescaler value to 1
	TWSR &= ~(_BV(TWPS1) | _BV(TWPS0));
	
	// Set Bit Rate register to calculated value for given SCL
	TWBR = (uint8_t) (F_CPU/scl -16)/2;
	
	busy = TWI_NOT_BUSY;

	return TWBR;
}

/**
 * Return the busy state of the TWI, that is if it is performing a transmission at the moment
 */

uint8_t i2c_status()
{
	return busy;
}

/**
 * Write as Master Transmitter
 */

uint8_t i2c_write_addr(uint8_t device, uint8_t address, uint8_t *wr_data, uint8_t wr_count)
{
	busy = TWI_BUSY;

	slave_address = device;
	register_address = address;
	wr_bytes = wr_data;
	wr_bytes_count = wr_count;
	
	//keep track of the current action that's being performed
	current_action = WRITING;
	
	/*
	// Initiate transmission
	TWCR = 1 << 7;
	*/
	
	// Enable TWI (TWEN), Interrupt(TWIE)
	TWCR |= _BV(TWEN) | _BV(TWIE);
	// Transmit START (TWSTA)
	TWCR |= _BV(TWSTA);
	
	// Clear Interrupt Flag (TWINT) to start transmission
	TWCR |= _BV(TWINT);
	
	return 0;
}

/**
 * Read as Master Receiver
 */

uint8_t i2c_read_addr(uint8_t device, uint8_t address, uint8_t *buffer, uint8_t max)
{
	busy = TWI_BUSY;

	
	slave_address = device;
	register_address = address;
	read_buffer = buffer;
	read_max = max;
	counter = 0;
		
	//keep track of the current action that's being performed
	current_action = READING;
		
	/*
	// Initiate transmission
	TWCR = 1 << 7;
	*/
	
	// Enable TWI (TWEN), Interrupt(TWIE)
	TWCR |= TWCR_DEFAULT|  _BV(TWSTA);

	return 0;
}

/**
 * ISR covering relevant status codes
 */

ISR(TWI_vect)
{
	// Writing
	if (current_action == WRITING)
	{
		switch (TWSR)
		{
			case (TW_START):
				// Send connection request to slave
				// Load SLA+W to TWDR
				// Slave address + W bit
				TWDR = 0;
				TWDR |= slave_address << 1;

				// Make sure that Start bit is clear
				TWCR = TWCR_DEFAULT | _BV(TWINT);

				break;
			
			case (TW_MT_SLA_ACK):
				// Select register address in the slave's memory space
				TWDR = register_address;
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
				
			case (TW_MT_SLA_NACK):
				// Set Stop bit
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				busy = TWI_NOT_BUSY;
				break;
			
			case (TW_MT_DATA_ACK):	
				if ((wr_bytes_count - 1 < 0))
				{
					// Stop transmission
					TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
					busy = TWI_NOT_BUSY;
					break;
				}

				// Load bytes to be transmitted, one at a time
				TWDR = wr_bytes[(wr_bytes_count -1)];
				wr_bytes_count--;
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
				
			case (TW_MT_DATA_NACK):
				// Set Stop bit
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				busy = TWI_NOT_BUSY;
				break;
	
			default:
				break;
		}
	} 
	else
	// Reading
	{
		switch (TWSR)
		{
			case (TW_START):
				// Send connection request to slave
				// Load SLA+W to TWDR
				// Slave address + W bit
				TWDR = 0;
				TWDR |= slave_address << 1;

				// Make sure that Start bit is clear
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
			
			case (TW_MT_SLA_ACK):
				// Select register address in the slave's memory space
				TWDR = register_address;
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
				
			case (TW_MT_SLA_NACK):
				// Set Stop bit
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				busy = TWI_NOT_BUSY;
				break;
			
			case (TW_MT_DATA_ACK):
				// Initiate repeated start
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTA);
				break;
	
			case (TW_REP_START):
				// Send connection request to slave
				// Load SLA+W to TWDR
				// Slave address + R bit
				TWDR = 1;
				TWDR |= slave_address << 1;
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
			
			case (TW_MR_SLA_ACK):
				// Select register address in the slave's memory space
				TWDR = register_address;
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
				
			case (TW_MR_SLA_NACK):
				// Set Stop bit
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				busy = TWI_NOT_BUSY;
				break;
			
			case (TW_MR_DATA_ACK):
				// Load bytes to be read, one at a time
				read_buffer[counter] = TWDR;
				counter++;
				read_max--;
				if (read_max - 1 < 0)
				{
					// Stop transmission
					TWCR = TWCR_NOT_ACK;
					break;
				}
				TWCR = TWCR_DEFAULT | _BV(TWINT);
				break;
			
			case (TW_MR_DATA_NACK):
				TWCR = TWCR_DEFAULT | _BV(TWINT) | _BV(TWSTO);
				busy = TWI_NOT_BUSY;
				break;
				
			default:
				break;
		}
	}
}