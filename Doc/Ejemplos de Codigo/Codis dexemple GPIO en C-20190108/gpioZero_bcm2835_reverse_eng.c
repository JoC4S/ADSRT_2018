/*
 * gpio_bcm2835_reverse_eng.c
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
 */


//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program for debugging register values
//  Use BCM pin numbers from 0 to 27.
//

// Access from ARM Running Linux

// Note: For Raspberry Pi 2 and Pi 3, change BCM2708_PERI_BASE to 0x3F000000 for the code to work.
//

#define BCM2708_PERI_BASE        0x20000000    	// Uncomment For Raspberry Pi 1 & Zero W
//#define BCM2708_PERI_BASE        0x3F000000		// Uncomment For Raspberry Pi 2 & 3

#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

void setup_io();

void minidelay()
{
	for (int i=0;i<150;i++);
}

int main(int argc, char **argv)
{
  int g,rep;

  // Set up gpi pointer for direct register access
  setup_io();

  // Switch GPIO 7..11 to output mode

 /************************************************************************\
  * You are about to change the GPIO settings of your computer.          *
  * Mess this up and it will stop working!                               *
  * It might be a good idea to 'sync' before running this program        *
  * so at least you still have your code changes written to the SD-card! *
 \************************************************************************/

// test de les dades

  for (g=17; g<=27; g=g+5)  //activa els leds
  {
    printf("amb g=%d l'adreça de INP_GPIO és %X, valor %x, nou valor %x, valor final %x\n",g,(unsigned)(gpio+((g)/10)),*(gpio+((g)/10)),~(7<<(((g)%10)*3)), *(gpio+((g)/10))&~(7<<(((g)%10)*3)) );
    printf("el valor de OUT_GPIO %x, nou valor %x, valor final %x\n",*(gpio+((g)/10)),(1<<(((g)%10)*3)), *(gpio+((g)/10))|(1<<(((g)%10)*3)) );
    printf("el valor de GPIO_SET %x, bit %x\n",*(gpio+7),(1<<g) );
    printf("el valor de GPIO_CLR %x, bit %x\n",*(gpio+10),(1<<g) );
    printf("el valor de GET_GPIO %x, bit %x, valor %X\n",*(gpio+13),(1<<g), (*(gpio+13)&(1<<g)));
    printf("el valor de GPIO_PULL %x, bit %x\n",*(gpio+37),(1<<g) );
    printf("el valor de GPIO_PULLCLK0 %x, bit %x\n",*(gpio+38),(1<<g) );

	INP_GPIO(g);
    printf("amb g=%d nou valor després de INP_GPIO %x\n",g,*(gpio+((g)/10)) );
	
    OUT_GPIO(g);
    printf("amb g=%d nou valor després de OUT_GPIO %x\n",g,*(gpio+((g)/10)) );

	printf("\n");
  }	

    printf("valor dels ports %x\n",*(gpio+13) );

  for (g=17; g<=27; g=g+5)
  {

    GPIO_SET = 1<<g;
    sleep(1);
    printf("amb g=%d nou valor després de INP_GPIO %x, bit actiu %x\n",g,*(gpio+13),GET_GPIO(g) );
	
    GPIO_CLR = 1<<g;
    sleep(1);
    printf("amb g=%d nou valor després de OUT_GPIO %x, bit actiu %x\n",g,*(gpio+13),GET_GPIO(g) );

	printf("\n");
  }	


// activar les resistències de pull-up dels polsadors 23 i 24
	GPIO_PULL=2;
	minidelay();
	GPIO_PULLCLK0=0x01800000;
	minidelay();
	GPIO_PULL=0;
	GPIO_PULLCLK0=0x00000000;
	INP_GPIO(23);
	INP_GPIO(24);


	for (int n=1;n<=10;n++)
	{
		GPIO_SET = 1<<22;  //activa el LED blanc
		if (!GET_GPIO(23)) // si es polsa val 0, sino !=0
			{
				printf("Button 23 pressed!\t");
				GPIO_SET = 1<<27;  //activa led groc
			}
		else // port is LOW=0V
			{
				printf("Button 23 released!\t");
				GPIO_CLR = 1<<27;  //apaga led groc
			}
		if (!GET_GPIO(24)) // si es polsa val 0, sino !=0
			{
				printf("Button 24 pressed!\n");
				GPIO_SET = 1<<17;  //activa led groc
			}
		else // port is LOW=0V
			{
				printf("Button 24 released!\n");
				GPIO_CLR = 1<<17;  //apaga led groc
			}
		sleep(1);
		
		GPIO_CLR = 1<<22;  //apaga el LED blanc
		if (!GET_GPIO(23)) // si es polsa val 0, sino !=0
			{
				printf("Button 23 pressed!\t");
				GPIO_SET = 1<<27;  //activa led groc
			}
		else // port is LOW=0V
			{
				printf("Button 23 released!\t");
				GPIO_CLR = 1<<27;  //apaga led groc
			}
		if (!GET_GPIO(24)) // si es polsa val 0, sino !=0
			{
				printf("Button 24 pressed!\n");
				GPIO_SET = 1<<17;  //activa led groc
			}
		else // port is LOW=0V
			{
				printf("Button 24 released!\n");
				GPIO_CLR = 1<<17;  //apaga led groc
			}
		sleep(1);
	}


  return 0;

} // main


//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }
	printf("el punter a /dev/mem és %d \n",mem_fd);

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

	printf("el punter a gpio_map de /dev/mem és %X amb l'adreça de base %X \n",(unsigned)gpio_map,GPIO_BASE);


   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
	printf("el punter a gpio és %X \n\n",(unsigned)gpio);


} // setup_io
