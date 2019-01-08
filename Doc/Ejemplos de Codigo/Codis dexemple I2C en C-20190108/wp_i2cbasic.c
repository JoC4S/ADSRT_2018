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
* $: gcc i2cbasic.c -o i2cbasic -lwiringPi
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

// access to reg #00H to configure normal operation
	result = wiringPiI2CWriteReg8(fd, 0x00, 0x01 );
		if(result == -1)
		{	
			printf("Writing Error");
		}
		
// access to control reg #13H to activate LEDs 1-4  	
	result = wiringPiI2CWriteReg8(fd, 0x13, 0x0F );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
// access to control reg #15H to activate LEDs 16-18  	
	result = wiringPiI2CWriteReg8(fd, 0x15, 0x38 );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	
// Configure PWM value of each LED,  
	result = wiringPiI2CWriteReg8(fd, 0x01, 0x0F ); // LED1 -> 015/255
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x02, 0x02 ); // LED2 -> 002/255
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x03, 0x41 ); // LED3 -> 065/255
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x04, 0x84 ); // LED4 -> 132/255
		if(result == -1)
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x10, 0x10 ); // LED16 -> 016/255
		if(result == -1)							// Red
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x11, 0x40 ); // LED17 -> 064/255
		if(result == -1)							// Blue
		{
			printf("No he pogut escriure");
		}
	result = wiringPiI2CWriteReg8(fd, 0x12, 0x50 ); // LED18 -> 080/255
		if(result == -1)							// Green
		{
			printf("No he pogut escriure");
		}


// Update LED's registers by writing any value on reg #16H
	result = wiringPiI2CWriteReg8(fd, 0x16, 0x00 );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}      
		
	usleep(2000000);
// Reset device
	result = wiringPiI2CWriteReg8(fd, 0x00, 0x00 );
		if(result == -1)
		{
			printf("No he pogut escriure");
		}      
	
	
}

