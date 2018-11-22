/***************************************************************************
                          libmail.c  -  client
                             -------------------
    begin                : mie Nov  7 16:00:04 CET 2018
    copyright            : (C) 2002 by J. Cayero
    email                :
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
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "libmail.h"

struct sockaddr_in serverAddr;
char serverName[] = "127.0.0.1";                                                //Adreça IP on està el client
int sockAddrSize;
int sFd;
int result;
char buffer[256];
/** Comando de finalización de correo.*/

/*!
   \brief Funcion para envio y recepción de una cadena de texto a traveś de socket
   \param "char* msg: Mensaje que se desea enviar a traves del socket."
   \param "int opcion": "1" Se espera respuesta, "0" No se espera respuesta
   \pre "Definir parametros de correo"
   \return "void"
 */
void sendTCPData (int opcion, char *msg){

	int result = 0;
	/*Enviar*/
	result = write(sFd, msg, strlen(msg));
	printf("SENT (bytes %d): %s\n", result, msg);
	/*Rebre si opción = 0*/
	if (!opcion) {
		memset(buffer,'\0',256);
		result = read(sFd, buffer, 256);
		printf("RECV(bytes %d): %s\n", result, buffer);
		if (result <0) {
			printf ("\n¡Error en recepción!.\n");
			exit(-1);
		}
	}
}
/************************
* tcpClient
************************/
int sendmail(char *mailto, char *texto_a_enviar){

	/*Crear el socket*/
	sFd=socket(AF_INET,SOCK_STREAM,0);

	/*Construir l'adreça*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	/*Conexió*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	if (result < 0)
	{
		printf("Error en establir la connexió\n");
		exit(-1);
	}
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
	/*Rebre*/
	memset(buffer,'\0',256);
	result = read(sFd, buffer, 256);
	printf("RECV(bytes %d): %s\n",   result, buffer);

/** Envio de correo:*/

	sendTCPData(WAITFOR_ACK,ehlo);

	/*Enviar Solicitud de autorizacion de usuario*/
	sendTCPData(WAITFOR_ACK,authlogin);

	/*Enviar usuario encriptado*/
	sendTCPData(WAITFOR_ACK,mailuser64);

	/*Enviar pass usuario encriptado*/
	sendTCPData(WAITFOR_ACK,userpass64);

	/*Enviar remitente*/
	sendTCPData(WAITFOR_ACK,mailfrom);

	/*Enviar Destinatario*/
	sendTCPData(WAITFOR_ACK,mailto);

	/*Enviar comando inicio envio de cuerpo del mail*/
	sendTCPData(WAITFOR_ACK,DATA);

	/*Enviar Asunto del correo*/
	char mailContent [strlen(mailSubject) + strlen(texto_a_enviar)];
	strcpy(mailContent,mailSubject);
	strcat(mailContent,texto_a_enviar);
	sendTCPData(NOACKWAIT,mailContent);

	/*Enviar Fin del mensaje*/
	sendTCPData(WAITFOR_ACK, endOfmail);

	close(sFd);

	return 0;
}
