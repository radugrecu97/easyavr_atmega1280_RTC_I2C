/*
 * DS1337_driver.h
 *
 * Created: 13-May-19 12:50:12
 *  Author: radug
 */ 


#ifndef DS1337_DRIVER_H_
#define DS1337_DRIVER_H_

uint8_t RTC_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
uint8_t RTC_set_date(uint8_t year, uint8_t month, uint8_t day);
uint8_t RTC_read_time(uint8_t *hour_buffer, uint8_t *min_buffer, uint8_t *sec_buffer);
uint8_t RTC_read_date(uint8_t *year_buffer, uint8_t *month_buffer, uint8_t *day_buffer);
uint8_t RTC_set_alarm(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t dy_dt);
uint8_t get_alarm_status();
void clear_alarm_status();

#endif /* DS1337_DRIVER_H_ */