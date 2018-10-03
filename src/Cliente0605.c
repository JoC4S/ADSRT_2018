/***************************************************************************
                          exemple_menu_consola.c
                             -------------------
    begin                : 2012
    copyright            : (C) 2010 by A. Moreno
    email                : amoreno@euss.cat
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		500
#define SERVER_PORT_NUM		5001

/******ImprimirMenu******************************/


void ImprimirMenu(void)
{
	printf("\n\nMenu:\n");
	printf("--------------------\n");
	printf("1: Solicitar Media\n");
	printf("2: Solicitar Maximo\n");
	printf("3: Solicitar Mínimo\n");
	printf("4: Reset Máximo-Mínimo\n");
	printf("5: Numero de Muestras\n");
	printf("6: Iniciar Adquisicion\n");
	printf("s: Sortir\n");
	printf("--------------------\n");
}

/********EnviarMsg*******************************/

int EnviarMsg(char const *missatge){
	struct sockaddr_in	serverAddr;
	char	    serverName[] = "127.0.0.1"; //AdreÃ§a IP on està  el servidor
	int			sockAddrSize;
	int			sFd;
	int 		result;
	char		buffer[256];

	/*Crear el socket*/
	sFd=socket(AF_INET,SOCK_STREAM,0);

	/*Construir l'adreÃ§a*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	/*ConexiÃ³*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	if (result < 0)
	{
		printf("Error en establir la connexiÃ³\n");
		exit(-1);
	}
	printf("\nConnexiÃ³ establerta amb el servidor: adreÃ§a %s, port %d\n",	inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	/*Enviar*/
	strcpy(buffer,missatge); //Copiar missatge a buffer
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n",	result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n",	result, buffer);

	/*Tancar el socket*/
	close(sFd);

	if (buffer[1] == '2') return -1;

	else return 0;
	}


/********* main**********************************/

int main(int argc, char **argv)
{
	char input;
	char *msg1="AUZ",*msg2="AXZ",*msg3="AYZ",*msg4="ARZ",*msg5="ABZ", msg6[8];
	int adq, temps,starStop;

	ImprimirMenu();
	input = getchar();

	while (input != 's')
	{
		switch (input)
		{
			case '1':
				printf("Heu seleccionat l'opció 1\n");
				EnviarMsg (msg1);
				break;
			case '2':
				printf("Heu seleccionat l'opció 2\n");
				EnviarMsg (msg2);
				break;
			case '3':
				printf("Heu seleccionat l'opció 3\n");
				EnviarMsg (msg3);
				break;
			case '4':
				printf("Heu seleccionat l'opció 4\n");
				EnviarMsg (msg4);
				break;
			case '5':
				printf("Heu seleccionat l'opció 5\n");
				EnviarMsg (msg5);
				break;
			case '6':
				printf("Heu seleccionat l'opció 6\n");
				printf("Seleccioneu Inici(1)/Parada(0)d'adquisicio: ");
				scanf("%i",&starStop);
				if (starStop == 1)
				{
					printf("Seleccioneu el temps d'aquisició (1-20s): ");
					scanf("%i",&temps);
					printf("Seleccioneu el numero de mostres per fer promig(1-9): ");
					scanf("%i",&adq);
					if (temps <21 && temps >0  && adq > 0 && adq < 10)
					{
						sprintf(msg6,"AM1%02i%iZ",temps,adq);
						EnviarMsg (msg6);
					}
					else printf ("Parametros incorrectos");
				}
				else
				{
					sprintf(msg6,"AM0001Z");
					EnviarMsg (msg6);
				}
				break;
			case 0x0a: //Això és per enviar els 0x0a (line feed) que s'envia quan li donem al Enter
				break;
			default:
				printf("Opció incorrecta\n");
				printf("He llegit 0x%hhx \n",input);
				break;
		}

		input = getchar();
		ImprimirMenu();

	}

	return 0;
}
