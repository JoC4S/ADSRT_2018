/***************************************************************************
                          main.c  -  server
                             -------------------
    begin                : lun feb  4 15:30:41 CET 2002
    copyright            : (C) 2002 by A. Moreno
    email                : amoreno@euss.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "func.h"

#define SERVER_PORT_NUM		5001
#define SERVER_MAX_CONNECTIONS	4

#define REQUEST_MSG_SIZE	1024

void respuestas (char buffer[256], int *ultimaMuestra, char missatge[255] , int fd, char buf[255], float arrLecturas[3600]);
int SendcommSerie(int fd, char  *missatgeSerie);
int recieveCommSerie(int fd, char buf[256]);
void TancarSerie(int fd);

#define COLOR_GREEN  "\033[32m"
#define COLOR_RED  "\031[43m"
#define COLOR_RESET "\033[0m"

//Variables para configuracion puerto Serie y la fucnion de commSerie
#define BAUDRATE B9600
//#define MODEMDEVICE "/dev/ttyS0"       	 //Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"        	 //Conexió directa PC(Linux) - Arduino
#define _POSIX_SOURCE 1 					 // POSIX compliant source

struct termios oldtio,newtio;

int	ConfigurarSerie(int fd);
int adquirirMedia ( int *ultimaMuestra, int fd, char buf[255], float arrLecturas[3600]);

int tomadeMuestras = 0;
int tiempoMuestreo = 0;
int fd = 0;									//identificador del puerto serie
char buf[255];  							//usada para almacenar el buffer en la funcion commSerie
float arrLecturas[3600];					//Array para almacenar las lecturas de temperatura
int ultimaMuestra = 0;						//Indice de arrayLecturas
pthread_mutex_t varmutex;					//Variable para mutex

void *trhead_solicitudMedias ()
{
	char led = '0';
	char msgled[7];
	printTime();printf (COLOR_GREEN"Esperando comunicacion Serie\n"COLOR_RESET);
	while(1)
	{
		if (tomadeMuestras == 1 )
		{
			if (led == '0')
				led = '1';
			else
				led ='0';
			sprintf(msgled,"AS13%cZ",led);
			adquirirMedia (&ultimaMuestra,fd, buf, arrLecturas);

			//Encendemos o apagamos led
			SendcommSerie(fd,msgled);
			recieveCommSerie(fd,buf);
		}
		sleep(tiempoMuestreo);
	}
	//pthread_exit(&j);
	return 0;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in	serverAddr;
	struct sockaddr_in	clientAddr;
	unsigned int			sockAddrSize;
	int			sFd;
	int			newFd;
	int 		result;

	char buffer[256];													//Almacena el buffer en la comunicacion TCP/IP
	char missatge[255];

	pthread_t  threadId;												//Se le da identificador el thread
	pthread_mutex_init(&varmutex, NULL);

	//Inicializamos el array de Lecturas a cero
	inicializaArray (arrLecturas);

	/*Preparar l'adreça local*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize);							//Posar l'estructura a zero
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/*Crear un socket*/
	sFd=socket(AF_INET, SOCK_STREAM, 0);

	/*Nominalitzar el socket*/
	result = bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize);

	/*Crear una cua per les peticions de connexió*/
	result = listen(sFd, SERVER_MAX_CONNECTIONS);

	//Abrimos canal comunicacion Serie
	fd = ConfigurarSerie(fd);

	//Se crea el trhead de ejecucion de comunicacion Serie
	pthread_create (&threadId, NULL, trhead_solicitudMedias, NULL);


	/*Bucle s'acceptació de connexions*/
	while(1)
	{

		printTime();printf(COLOR_RESET"Servidor esperant connexions\n");

		/*Esperar conexió. sFd: socket pare, newFd: socket fill*/
		newFd = accept(sFd, (struct sockaddr *) &clientAddr, &sockAddrSize);

		printTime();printf(COLOR_RESET"Connexión acceptada del client: adreça %s, port %d\n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		/*Rebre*/
		result = read(newFd, buffer, 256);

		printTime();printf(COLOR_RESET"Missatge rebut del client(bytes %d): %s\n",	result, buffer);

		if (result > 0) 														//Si hay entrada de datos por la comunicación TCP ejecutamos sus respuestas
			respuestas (buffer, &ultimaMuestra, missatge,fd,buf, arrLecturas);

		/*Enviar*/
		strcpy(buffer,missatge);												//Copiar missatge a buffer
		result = write(newFd, buffer, strlen(buffer)+1); 						//+1 per enviar el 0 final de cadena

		printTime();printf(COLOR_RESET"Missatge enviat a client(bytes %d): %s\n",result, missatge);

		/*Tancar el socket fill*/
		result = close(newFd);
		memset(buffer,'\0',256);												//Se borra buffer y missatge tras cerrarl el socket tcpip
		memset(missatge,'\0',255);
	}
	pthread_mutex_destroy( &varmutex );
	TancarSerie(fd);

}
//Configura y abre el puerto serie
int	ConfigurarSerie(int fd)
{

	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
	if (fd <0) {perror(MODEMDEVICE); printf("No se ha podido abrir el puerto");exit(-1); }

	tcgetattr(fd,&oldtio); /* save current port settings */

	bzero(&newtio, sizeof(newtio));
	//newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

 	//sleep(1); //Per donar temps a que l'Arduino es recuperi del RESET

	return fd;
}
//Cierra el puerto serie
void TancarSerie(int fd)
{
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
}
//Funcion para comunicacion Serie : commSerie( char *msg_to_send, int fd)
int SendcommSerie(int fd, char  *missatgeSerie)
{
	int i = 0, res = 0;
	//Bloqueamos acceso al puerto Serie al resto de threads y procesos.
	pthread_mutex_lock( &varmutex );

	// Enviar el missatge 1
	res = write(fd,missatgeSerie,strlen(missatgeSerie));

	if (res <0) {tcsetattr(fd,TCSANOW,&oldtio); perror(MODEMDEVICE); exit(-1); }

	//Mostramos por pantalla el mensaje y los bytes enviados.
	printTime(); printf(COLOR_GREEN"Enviats Serie %d bytes: ",res);
	for (i = 0; i < res; i++)
	{
		printf("%c",missatgeSerie[i]);
	}
	printf("\n");

	return 0;
}
//Funcion para la recepcion por Serie : recieveCommSerie(int fd, char buf)
int recieveCommSerie(int fd, char buf[256])
{
	int i = 0, errorMsg = 0, bytes = 0;

	//Se borra el buffer Serie antes de iniciar la recepción.
	memset(buf,'\0',256);

	//Leemos el buffer de entrada de uno en uno hasta tener un mensaje completo.
	while (!bytes)
	{
		ioctl(fd, FIONREAD, &bytes);
		while (bytes > 0)
		{
			read(fd,buf+i,1);
			bytes--;
			i++;
		}
		if (buf[0] == 'A' && (buf[3] == 'Z' || buf[7] == 'Z'))
		{

			printTime();printf (COLOR_GREEN"Rebut Serie %ld bytes: %s", strlen(buf),buf);
			break;
		}
	}
	printf(COLOR_RESET "\n");

	//Se libera el puerto Serie
	pthread_mutex_unlock( &varmutex );

	return errorMsg;
}

//Funcion para la adquisicion de la media del Arduino
int adquirirMedia ( int *ultimaMuestra, int fd, char buf[255], float arrLecturas[3600])
{
		int errorRecepcion = 0;
		//Se solicita la Media a Arduino por comm Serie y se espera la respuesta
		SendcommSerie(fd, "ACZ");
		errorRecepcion = recieveCommSerie(fd, buf);

		//Convertimos el valor de la media de cadena de caracteres a integuer
		int mediaRecibida =(buf[3]-48)*1000 +(buf[4]-48)*100 +(buf[5]-48)*10 + (buf[6]-48);

		//Calculamos la temperatura multiplicando el integuer por la sensibilidad del sensor
		float temperatura = mediaRecibida * 0.048;

		//Si la recepcion Serie ha sido correcta, guardamos la muestra. En caso de error, no se guardara el dato.
		if (errorRecepcion == 0 && (mediaRecibida > 0 && mediaRecibida < 1024))
		{
			llenarArray (arrLecturas, *ultimaMuestra,temperatura);
			*ultimaMuestra = *ultimaMuestra + 1;
			if (*ultimaMuestra > 3599) *ultimaMuestra = 0;
		}
		else
		{
			printTime();printf ("NO SE GUARDARA EL DATO RECIBIDO");
			temperatura = -1;
		}
		return temperatura;
}

//Respuestas a las peticiones del Cliente
void respuestas (char buffer[256], int *ultimaMuestra, char missatge[255] , int fd, char buf[255], float arrLecturas[3600])
{
	/********************Integramos la respuesta de los mensajes *******************************/
	if (buffer[0]== 'A' && (buffer[2]== 'Z' || buffer[6]== 'Z' ))
	{
		int codigoRetorno = 0;
		float media = 0.0;
		switch (buffer[1])
		{
				case ('U'):																				// Solicitar la media("AUZ")
				{
					for (int i=0; i < *ultimaMuestra; i++)
						media += arrLecturas[i];

					media = media / *ultimaMuestra;
					printTime();sprintf(missatge,"AU%i%02.2fZ",codigoRetorno, media);
					break;
			    }
				case ('X'):																				// Solicitar Maximo ("AXZ")
				{
					float max = valorMaximo(arrLecturas,*ultimaMuestra);
					printTime();sprintf(missatge,"AX%i%02.2fZ",codigoRetorno,max);
					break;
				}
				case ('Y'):																				// Solicitar Minimo ("AYZ")
				{
					float min = valorminimo(arrLecturas,*ultimaMuestra);
					printTime();sprintf(missatge,"AY%i%02.2fZ",codigoRetorno,min);
					break;
				}
				case ('R'):																				// Reset Maximo y minimo ("ARZ")
				{
					inicializaArray (arrLecturas);
					*ultimaMuestra = 0;
					printTime();sprintf(missatge,"AX%iZ",codigoRetorno);
					break;
				}
				case ('B'):																				// Contador Muestras guardadas ("ARZ")
				{
					printf( COLOR_YELLOW "\n**********   DEBUG  *************\n");
					for (int i=0;i<*ultimaMuestra;i++)
					{
						printf("arrLecturas[%d]= %02.2f\n",i,arrLecturas[i]);
					}
					printf("\n*********  FIN DEBUG  ************\n" COLOR_RESET);
					printTime();sprintf(missatge,"AB%i%04iZ",codigoRetorno,*ultimaMuestra);
					break;
				}
				case ('M'):																				//	Marcha/Paro Adquisicion
				{
					if (codigoRetorno == 0 )
					{
						int tomadeMuestrasTemp = buffer[2]-48;
						int tiempoMuestreoTemp = (buffer[3]-48)*10 + buffer[4]-48;
						if (buffer[2]-48 == '0')									//En caso de orden de parada recibida
							{
								tomadeMuestras = 0;
								SendcommSerie(fd, "AM0001Z");
								recieveCommSerie(fd, buf);
							}
						else
						{
							SendcommSerie(fd, buffer);
							if (recieveCommSerie(fd, buf) != -1 )
							{
								tomadeMuestras = tomadeMuestrasTemp;
								tiempoMuestreo = tiempoMuestreoTemp;
							}
							else codigoRetorno = 2;
						}
					}
					printTime();sprintf(missatge,"AM%iZ",codigoRetorno);
					break;
				}
				default:
				{
					codigoRetorno = 2;
					break;
				}
		}
	}
}
