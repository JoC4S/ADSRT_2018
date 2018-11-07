/***************************************************************************
                          main.c  -  client
                             -------------------
    begin                : mie Nov  47 16:00:04 CET 2018
    copyright            : (C) 2002 by J. Cayero
    email                : jmcsaez@gmail.com
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

#define REQUEST_MSG_SIZE        1024
#define REPLY_MSG_SIZE          500
#define SERVER_PORT_NUM         25

struct sockaddr_in serverAddr;
char serverName[] = "127.0.0.1";                                                //Adreça IP on està el client
int sockAddrSize;
int sFd;
int result;
char buffer[256];
#define ehlo            "ehlo localhost\n"
#define authlogin       "auth login\n"
#define mailuser64      "am1jc2FlekBnbWFpbC5jb20K\n"                            /** usuario gmail encriptado*/
#define userpass64      "RXJydlQzVTMK\n"                                        /** password usuario gmail encriptado*/
#define mailfrom        "mail from: <user@euss.es>\n"                           /** Remitente*/
#define mailto          "RCPT TO: <1104934@campus.euss.org>\n"                  /** Destinatario*/
#define DATA            "DATA\n"                                                /** Comando de inicio de cuerpo del correo*/
#define mailSubject     "Subject: Ejemplo de comunicación a traves de mail."    /** Asunto del correo*/
#define endOfmail       "\r\n.\r\n"                                             /** Comando de finalización de correo.*/

/*!
   \brief Funcion para envio y recepción de una cadena de texto a traveś de socket
   \param "char* msg: Mensaje que se desea enviar a traves del socket."
   \pre "Definir parametros de correo"
   \return "void"
 */
void sendTCPData (char *msg){

	int result = 0;
	/*Enviar*/
	result = write(sFd, msg, strlen(msg));
	printf("SENT (bytes %d): %s\n", result, msg);
	/*Rebre*/
	memset(buffer,'\0',256);
	result = read(sFd, buffer, 256);
	printf("RECV(bytes %d): %s\n", result, buffer);
	if (result <0) {
		printf ("\n¡Error en recepción!.\n");
		exit(-1);
	}
}
/************************
* tcpClient
************************/
int sendmail(char *texto_a_enviar){

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

	sendTCPData(ehlo);

	/*Enviar Solicitud de autorizacion de usuario*/
	sendTCPData(authlogin);

	/*Enviar usuario encriptado*/
	sendTCPData(mailuser64);

	/*Enviar pass usuario encriptado*/
	sendTCPData(userpass64);

	/*Enviar remitente*/
	sendTCPData(mailfrom);

	/*Enviar Destinatario*/
	sendTCPData(mailto);

	/*Enviar comando inicio envio de cuerpo del mail*/
	sendTCPData(DATA);

	/*Enviar Asunto del correo*/
	sendTCPData(mailSubject);

	/*Enviar Cuerpo del mensaje*/
	//char mailtext[] = "\nCuerpo del mail de prueba.\nAdios.";
	sendTCPData(texto_a_enviar);

	/** Finalizar envio de correo*/
	sendTCPData(endOfmail);

	close(sFd);

	return 0;
}
