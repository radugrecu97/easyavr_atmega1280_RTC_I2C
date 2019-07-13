/*
 * i2c_driver.h
 *
 * Created: 10-May-19 11:16:24
 *  Author: radug
 */ 


#ifndef I2C_H_DRIVER_
#define I2C_H_DRIVER_

#ifndef F_CPU
	#define F_CPU 7372800L
#endif

uint8_t i2c_init(long scl);
uint8_t i2c_status();
uint8_t i2c_write_addr(uint8_t device, uint8_t address, uint8_t *wr_data, uint8_t wr_count);
uint8_t i2c_read_addr(uint8_t device, uint8_t address, uint8_t *buffer, uint8_t max);

#endif /* I2C_H_DRIVER_ */