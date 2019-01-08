/***************************************************************************
                          exemple_led_rpi.c
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by S. Bernadas
    email                : sbernadas@euss.cat
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   compile with:
 *   $ gcc -o exemple_led_rpi exemple_led_rpi.c
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  
#include <string.h>                                                      

#define EXPORT "/sys/class/gpio/export"
#define UNEXPORT "/sys/class/gpio/unexport"

#define LEDWHITE "/sys/class/gpio/gpio22/value"
#define LEDRED "/sys/class/gpio/gpio17/value"
//#define LEDGREEN "/sys/class/gpio/gpio24/value"
#define LEDYELLOW "/sys/class/gpio/gpio27/value"

#define D_LEDWHITE "/sys/class/gpio/gpio22/direction"
#define D_LEDRED "/sys/class/gpio/gpio17/direction"
//#define D_LEDGREEN "/sys/class/gpio/gpio24/direction"
#define D_LEDYELLOW "/sys/class/gpio/gpio27/direction"


/************************
*
*
* Controlar LED's placa
*
*
*/

void led_on(char addr[])
{
	int fd;
	char m[] = "1";

	fd = open(addr, O_WRONLY);

	if (fd < 0) {perror("Error a l'obrir el dispositiu\n"); exit(-1);} 

	write(fd,m,1);
	
	close(fd);
}

void led_off(char addr[])
{
	int fd;
	char m[] = "0";

	fd = open(addr, O_WRONLY); 

	if (fd < 0) {perror("Error a l'obrir el dispositiu\n"); exit(-1);} 

	write(fd,m,1);
	
	close(fd);
}

/*************
 * Funció que escriu el valor al fitxer.
 * write to file this value.
 */

void wfv(char addr[], char message[])
{
	int fd;

	fd = open(addr, O_WRONLY); 
	char m_error[200];
	sprintf(m_error,"Error a l'obrir el dispositiu %s\n",addr);

	if (fd < 0) {perror(m_error); exit(-1);} 

	write(fd,message,strlen(message));
//	printf("missatge %s, mida %d\n",message, strlen(message));
	
	close(fd);
	}

/************
 * inicialitzar els gpio que es faran servir
 */

void setup_gpio ()
{
	int fdtest,n=10000;
	
	wfv(EXPORT,"22");
	wfv(EXPORT,"27");
//	wfv(EXPORT,"24");
	wfv(EXPORT,"17");
	

//espera a que s'hagin creat tots els dispositius.

	do{
		n--;
		fdtest = open(LEDRED, O_WRONLY); 
	}
	while((n>0) && (fdtest<0));
	printf("---> Export Ok %d %d \n",n, fdtest);

	do{
		n--;
		fdtest = open(LEDYELLOW, O_WRONLY); 
	}
	while((n>0) && (fdtest<0));
	printf("---> Export Ok %d %d \n",n, fdtest);

	do{
		n--;
		fdtest = open(LEDWHITE, O_WRONLY); 
	}
	while((n>0) && (fdtest<0));
	printf("---> Export Ok %d %d \n",n, fdtest);


//defineix la direcció de sortida dels LEDs
	wfv(D_LEDYELLOW,"out");
	wfv(D_LEDWHITE,"out");
//	wfv(D_LEDGREEN,"out");
	wfv(D_LEDRED,"out");

}

/**************
 * allivera els gpio
 */

void free_gpio ()
{
	wfv(UNEXPORT,"22");
	wfv(UNEXPORT,"27");
//	wfv(UNEXPORT,"24");
	wfv(UNEXPORT,"17");

}

/************************
*
*
* main
*
*
*/
int main(int argc, char *argv[]){
	
	setup_gpio();
	
	printf("Encendre el led vermell\n");
	led_on(LEDRED);
	
	sleep(1);

	printf("Apagar el led vermell\n");
	led_off(LEDRED);

	sleep(1);
	
	printf("Encendre el led vermell i groc\n");
	led_on(LEDRED);
	led_on(LEDYELLOW);
	sleep(1);

	printf("Encendre el led blanc i apagar el groc\n");
	led_off(LEDYELLOW);
	led_on(LEDWHITE);
	sleep(1);

	printf("Apagar el led vermell i el blanc\n");
	led_off(LEDRED);
	led_off(LEDWHITE);
	
	free_gpio ();
	
	return 0;
	}
