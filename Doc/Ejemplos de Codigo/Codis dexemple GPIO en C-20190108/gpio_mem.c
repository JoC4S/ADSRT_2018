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
// Note: For Raspberry Pi 2 and Pi 3, change BCM2708_PERI_BASE 
// to 0x3F000000 for the code to work.
//
// If you know the machine, you can use defines instead autoconfiguration in
//#define BCM2708_PERI_BASE        0x20000000    	// Uncomment For //Raspberry Pi 1 & Zero W
//#define BCM2708_PERI_BASE        0x3F000000		// Uncomment For //Raspberry Pi 2 & 3

#define GPIO_BASE    (v_BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
#define N_BUFFER 32

int  mem_fd;
FILE *fp;
void *gpio_map;
char buffer[N_BUFFER];
unsigned v_BCM2708_PERI_BASE=0x3F000000;

// I/O access
volatile unsigned *gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or     // SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  //sets bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) //clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

void setup_io();


int main(int argc, char **argv)
{
  int g;

  system("uname -m > machine");
  fp=fopen("machine","r");
  if(fp) 
	{
		fgets(buffer,N_BUFFER,fp);
		fclose(fp);
		printf(" --> La màquina és %s\n",buffer);
		if (buffer[4]=='6') v_BCM2708_PERI_BASE=0x20000000;
		printf("Peri_base %x GPIO_base %x\n\n",v_BCM2708_PERI_BASE,GPIO_BASE);
	}
  else
	{
		perror("Error opening file\n");
		return(-1);
	}
	
  // Set up gpi pointer for direct register access
  setup_io();

  // Switch GPIO 13, 16 & 19 to output mode
 /************************************************************************
  * You are about to change the GPIO settings of your computer.          *
  * Mess this up and it will stop working!                               *
  * It might be a good idea to 'sync' before running this program        *
  * so at least you still have your code changes written to the SD-card! *
  ************************************************************************/
  for (g=13; g<=19; g=g+3)
  {
    INP_GPIO(g);  // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(g);
  }	

  for (g=13; g<=19; g=g+3)
  {
    GPIO_SET = 1<<g;
    sleep(1);
    GPIO_CLR = 1<<g;
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

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
} // setup_io
