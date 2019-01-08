/******************************************************************************
* i2ctest.c
*
* Copyright 2018  <sbernadas@euss.es>
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* Raspberry Pi I2C interface demo for all LEDs attached to 
* SN3218 18 channel LED driver I2C Device
*
* This example makes use of the Wiring Pi library, which streamlines the interface
* the the I/O pins on the Raspberry Pi, providing an API that is similar to the
* Arduino.  
*
* The board attached is Automation HAT
*
* To build this file use 
* $: gcc i2ctest.c -o i2ctest -lwiringPi
*
* To run it the I2C kernel module needs to be loaded.  
* This can be done using the GPIO utility if necessary 
* $: gpio load i2c
******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <wiringPiI2C.h>


int main()
{
	int fd, result;

// setup for the address of the I2C device
	fd = wiringPiI2CSetup(0x54);
	if (fd < 0) return fd;

// Reset de device, clean start
	result = wiringPiI2CWriteReg8(fd, 0x17, 0x00 );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
		
// access to reg #00H to configure normal operation
	result = wiringPiI2CWriteReg8(fd, 0x00, 0x01 );
		if(result == -1)
		{	
			printf("Writing Error");
		}
		
// access to control regs #13H, #14H & #15H to activate each LED  	
	result = wiringPiI2CWriteReg8(fd, 0x13, 0x3F );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x14, 0x3F );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x15, 0x3F );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	
// Sequentially & gradually activate each LED   
	for (int n=0; n<=9; n++) // LED luminous intensity
	for (int led=1; led <=18; led++) // LED number
	{

	// Configure PWM value of each LED, 0 off, 9 also off
	// n=1 -> 1, n=2 -> 2, n=3 -> 4, n=4 -> 8, n=5 -> 16... n=8 -> 128 
	result = wiringPiI2CWriteReg8(fd, led, (n? 1<<(n-1):0) );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	// Update LED's registers by writing any value on reg #16H
	result = wiringPiI2CWriteReg8(fd, 0x16, 0x00 );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	usleep (50000);   
      
   }   
      
}

