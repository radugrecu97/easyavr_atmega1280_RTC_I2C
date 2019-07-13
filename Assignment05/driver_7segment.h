/*
 * driver_7segment.h
 *
 * Created: 06-Apr-19 16:43:00
 *  Author: radug
 */ 


#ifndef DRIVER_7SEGMENT_H_
#define DRIVER_7SEGMENT_H_

void init_7_segment();
void printint_7_segment(uint16_t value);
void display_7_segment(float value, uint8_t no_of_decimals);

#endif /* DRIVER_7SEGMENT_H_ */