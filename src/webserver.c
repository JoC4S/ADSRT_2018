/***************************************************************************
              Exemple Senzill Servidor Web = Servidor HTTP(HTML)
                             -------------------
    begin                : Monday Nov 23 19:30:00 CET 2015
    copyright            : (C) 2015 by A. Fontquerni
    email                : afontquerni@euss.cat
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// max size
#define BUFFSIZE 1024*1000

#define TCPDEBUG(x...) printf(x)
//#define TCPDEBUG(x...)

int global_mesura = 0;

int tcp_cmd_captura( int newsockfd, char *buffer)
{
	int result;
	char resposta[BUFFSIZE];
	bzero(resposta,BUFFSIZE);


	TCPDEBUG("Found LOCAL URL webservice:[%s]\n", "captura.html" );

	/*
	HTTP/1.1 200 OK
	Date: Wed, 01 Jun 2011 18:36:38 GMT
	Server: Apache/2.2.14 (Ubuntu)
	Content-Type: text/html

	<HTML>
	<HEAD>
	</HEAD>
	<BODY>
	...
	</BODY>
	</HTML>
	*/
	const char *httpheader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";

	const char example[] = "HTTP/1.1 200 OK\r\nDate: Wen, 2 Dec 2015 13:40:00 GMT\r\nContent-Type: application/html\r\nKeep-Alive: timeout=2, max=100\r\nConnection: Keep-Alive\r\n";

	const char *web="<html> \r\n\
	 <head> \r\n\
	  <title>ADST - Captura</title> \r\n\
	 </head> \r\n\
	 <body bgcolor=\"#FFFFC0\"> \r\n\
	  <h1 align=\"center\">Captura: %d</h1> \r\n\
	  Exemple de com funciona el <strong>servidor web</strong>. Hem incrementat<br>\r\n\
	  Per Incrementar cal fer click <a href=\"http://localhost:8000/captura.html\">aqui</a> <br>\r\n\
	  Per Decrementar cal fer click <a href=\"http://localhost:8000/decrementa.html\">aqui</a>\r\n\
	 </body> \r\n\
	</html>\r\n";

	global_mesura++;

	char htmlweb[BUFFSIZE];
	bzero(htmlweb,BUFFSIZE);
	sprintf(htmlweb, web, global_mesura);

	sprintf(resposta,"%sContent-Length: %d\r\n\r\n%s", httpheader, (int)strlen(htmlweb), htmlweb);

	result = write(newsockfd, resposta, strlen(resposta));
	TCPDEBUG("SENDED:[%s]\n", resposta );

	return result;
}
int tcp_cmd_fita3( int newsockfd, char *buffer)
{
	int result;
	char resposta[BUFFSIZE];
	bzero(resposta,BUFFSIZE);


	TCPDEBUG("Found LOCAL URL webservice:[%s]\n", "fita3.html" );

	/*
	HTTP/1.1 200 OK
	Date: Wed, 01 Jun 2011 18:36:38 GMT
	Server: Apache/2.2.14 (Ubuntu)
	Content-Type: text/html

	<HTML>
	<HEAD>
	</HEAD>
	<BODY>
	...
	</BODY>
	</HTML>
	*/
	const char *httpheader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";

	const char example[] = "HTTP/1.1 200 OK\r\nDate: Wen, 2 Dec 2015 13:40:00 GMT\r\nContent-Type: application/html\r\nKeep-Alive: timeout=2, max=100\r\nConnection: Keep-Alive\r\n";

	const char *web= ""<!DOCTYPE HTML>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>fascinating - HTML5 CANVAS 3D CUBES demo</title>
</head>
<body>
<div id="screen">
<canvas id="canvas">HTML5 CANVAS</canvas>
<div id="info">
	<div class="background"></div>
		<div class="content">
			<h1>3D Cubes</h1>
			<table>
				<tr><td class="w">drag</td><td>→ rotate X,Y axis</td></tr>
				<tr><td class="w">wheel</td><td>→ rotate Z axis</td></tr>
				<tr><td class="w">click</td><td>→ create cube</td></tr>
			</table>
			<hr>
			<input type="checkbox" id="white"><label for="white"> white background</label><br>
			<input type="checkbox" id="alpha"><label for="alpha"> transparency</label><br>
			<input type="checkbox" id="autor"><label for="autor"> auto rotation</label><br>
			<input type="checkbox" id="destroy"><label for="destroy"> destroy cubes</label><br>
			<hr>
			- <span id="fps" class="w">00</span> FPS<br>
			- <span id="npoly" class="w">00</span> Faces<br>
			<p align="center"><input type="button" value="RESET" id="reset" class="button"></input><input type="button" value="STOP" id="stopgo" class="button"></input></p>
		</div>
	</div>
</div>

</body>
</html>


	global_mesura++;

	char htmlweb[BUFFSIZE];
	bzero(htmlweb,BUFFSIZE);
	sprintf(htmlweb, web, global_mesura);

	sprintf(resposta,"%sContent-Length: %d\r\n\r\n%s", httpheader, (int)strlen(htmlweb), htmlweb);

	result = write(newsockfd, resposta, strlen(resposta));
	TCPDEBUG("SENDED:[%s]\n", resposta );

	return result;
}

int tcp_cmd_decrementa( int newsockfd, char *buffer)
{
	int result;
	char resposta[BUFFSIZE];
	bzero(resposta,BUFFSIZE);


	TCPDEBUG("Found LOCAL URL webservice:[%s]\n", "decrementa.html" );


	const char *httpheader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";

	//const char example[] = "HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Type: application/xml\r\n";
	const char example[] = "HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Type: application/xml\r\nKeep-Alive: timeout=2, max=100\r\nConnection: Keep-Alive\r\n";

	const char *web="<html> \r\n\
	 <head> \r\n\
	  <title>ADST - Captura</title> \r\n\
	 </head> \r\n\
	 <body bgcolor=\"#C0FFFF\"> \r\n\
	  <h1 align=\"center\">Captura: %d</h1> \r\n\
	  Exemple de com funciona el <strong>servidor web</strong>. Hem decrementat<br>\r\n\
	  Per Incrementar cal fer click <a href=\"http://localhost:8000/captura.html\">aqui</a> <br>\r\n\
	  Per Decrementar cal fer click <a href=\"http://localhost:8000/decrementa.html\">aqui</a>\r\n\
	 </body> \r\n\
	</html>\r\n";

	global_mesura--;

	char htmlweb[BUFFSIZE];
	bzero(htmlweb,BUFFSIZE);
	sprintf(htmlweb, web, global_mesura);

	sprintf(resposta,"%sContent-Length: %d\r\n\r\n%s", httpheader, (int)strlen(htmlweb), htmlweb);

	result = write(newsockfd, resposta, strlen(resposta));
	TCPDEBUG("SENDED:[%s]\n", resposta );

	return result;
}


int tcp_cmd_error( int newsockfd, char *buffer )
{
	int result;

	const char *error_http_msg="HTTP/1.1 404 Not Found \r\n\
Content-Type: text/html \r\n\
Content-Length: 345 \r\n\
Date: Thu, 01 Jan 1970 06:31:10 GMT \r\n\
Server: lighttpd/1.4.26 \r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"iso-8859-1\"?> \r\n\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \r\n\
         \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"> \r\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\"> \r\n\
 <head> \r\n\
  <title>404 - Not Found</title> \r\n\
 </head> \r\n\
 <body> \r\n\
  <h1>404 - Not Found</h1> \r\n\
 </body> \r\n\
</html>\r\n";
	bzero(buffer,BUFFSIZE);

	sprintf(buffer,"%s", error_http_msg );

	result = write(newsockfd,buffer,strlen(buffer));
	TCPDEBUG("Send NOTFOUND:[%s]\n", buffer );

	return result;
}

void tcp_error(const char *msg)
{
	printf("tcp_error:%s\n", msg );
	perror(msg);
	exit(1);
}

int tcp_open(int portno)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	 int optval; /* flag value for setsockopt */

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	tcp_error("ERROR opening socket");

	  /* setsockopt: Handy debugging trick that lets
	   * us rerun the server immediately after we kill it;
	   * otherwise we have to wait about 20 secs.
	   * Eliminates "ERROR on binding: Address already in use" error.
	   */
	  optval = 1;
	  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
			 (const void *)&optval , sizeof(int));

	bzero((char *) &serv_addr, sizeof(serv_addr));
	//portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0)
	      tcp_error("ERROR on binding");
	listen(sockfd,5);

	return sockfd;
}

int tcp_close(int sockfd)
{
	//TCPDEBUG( "close\n" );
	close(sockfd);
	return 0;
}


int tcp_service( int sockfd ) {

	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	char buffer[BUFFSIZE];
	int n, result;

	clilen = sizeof(cli_addr);


	newsockfd = accept(sockfd,
		 (struct sockaddr *) &cli_addr,
		 &clilen);

	if (newsockfd < 0) {
		tcp_error("ERROR on accept");
		return -1;
	}

	bzero(buffer,BUFFSIZE);
	result = read(newsockfd,buffer,BUFFSIZE-1);
	if (result < 0) tcp_error("ERROR reading from socket");
	else TCPDEBUG("READED:[%s]\n", buffer );

	// - Check URL
	if( strstr( buffer, "GET /captura.html" ) ) {
		// buidar l'array aquí i generar la cadena buffer de json
		TCPDEBUG("INC\n");
		result = tcp_cmd_captura( newsockfd, buffer);
	}
	if( strstr( buffer, "GET /fita3.html" ) ) {
		// buidar l'array aquí i generar la cadena buffer de json
		TCPDEBUG("INC\n");
		result = tcp_cmd_fita3( newsockfd, buffer);
	}
	else if( strstr( buffer, "GET /decrementa.html" ) ) {
		// buidar l'array aquí i generar la cadena buffer de json
		TCPDEBUG("DEC\n");
		result = tcp_cmd_decrementa( newsockfd, buffer);
	}
	else {
		result = tcp_cmd_error( newsockfd, buffer );
	}

	if (result < 0) tcp_error("ERROR writing to socket");
	close(newsockfd);

	return 0;

}

int main(int argc, char *argv[])
{
	int sockfd = 0;
	int result = 0;

	if(argc < 2) {
		printf("Usage: webserver port\n");
		return -1;
	}
	sockfd = tcp_open(atoi(argv[1]));
	int num_connection = 1;

	do {
		TCPDEBUG("tcp_service(%03d): Starts...\n", num_connection);
		result = tcp_service(sockfd);
		TCPDEBUG("tcp_service(%03d): ...Ends\n", num_connection);

		TCPDEBUG("\n\n=============================================\n\n");

		num_connection++;
	} while	(result==0);

	tcp_close(sockfd);
}
