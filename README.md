## Before you start:

Make sure the switches are set in the correct state as shown in the diagram, otherwise the specified peripheral won't work correctly or at all.

It is assumed that the reader has some knowledge of port pin configurations and electronics.

## 7 Segment

### Note!

This is a driver for 74HC595 not ATmega 1280's SPI!
The slave select pin is not utilised as Slave to Master communication is not of interest.

The 0-3 pins of port D determine which segment will be lit up. Only one at a time must be driven high.
To print a number with at least 2 digits, the segments will be lit up in sequence. The greater the number of digits, the higher the refresh frequency of the display segment should be in order to provide seamless refreshing. 

### Display updating/refreshing

The display is updated periodically with the use of a timer interrupt. To tune the update frequency, the following formula is utilized:

![Prescaler_formula](assets/Prescaler_formula.PNG)

Where:
N - prescaler
OCR - the value that the timer will count to and once reached, will trigger an interrupt
fclk - CPU frequency (which is 7372800 for ATmega1280)

### Pushing data

The following timing diagram can be used as a reference for implementing the SPI.
Note that the Master Reset (MR) is not used at all as it wasn't deemed necessary but it is held high as it's active low.

![wavedrom](assets/wavedrom.png)

![Datasheet_7_segment](assets/Datasheet_7_segment.PNG)


## I2C

The I2C states were taken from the ATmega 1280 datasheet.

![TWI_States](assets/TWI_States.png)

## RTC

Refer to the following table for the register addreses.

![TimeKeeper_Registers](assets/TimeKeeper_Registers.PNG)
