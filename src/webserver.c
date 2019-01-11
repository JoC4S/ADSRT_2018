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
#include <sqlite3.h>
#include <time.h>

int webfile_read(char *fichero, char *str);

#define COLOR_GREEN "\x1B[32m"
#define COLOR_RED "\x1B[31m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_RESET "\033[0m"
#define PRINT_TIME 1                            /** Para funcion timestamp(). Idica que se imprimira la hora por patnalla.*/


// max size
#define BUFFSIZE 1024 * 1000

#define TCPDEBUG(x ...) printf(x)
//#define TCPDEBUG(x...)

float global_angulo;

/*!
   \brief "Obtencion de fecha con formato sqlite DATETIME"
   \param "No tiene parametros"
   \return "Devuelve puntero a un string con la fecha"
 */
char* timestamp(int print)
{
	time_t rawtime;
	struct tm *info;
	char buffer[80];
	char *retp;

	time( &rawtime );

	info = localtime( &rawtime );
	//El valor de retorn es a una variable de tipus timei_t, on posaràl temps en segons des de 1970-01-01 00:00:00 +0000 (UTC)

	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);
	if (print) {
		printf(COLOR_YELLOW "%s" COLOR_RESET, buffer );
	}
	retp = buffer;
	return retp;
}

/*!
   \brief Obtiene el dato máximo de ángulo y lo convierte en float para pasarlo a la vriable global
   \param "Param description"
   \return "Return of the function"
 */
int callback_angulo(void *NotUsed, int argc, char **argv, char **azColName)
{
	char angulo[100];
	FILE *fp;

	bzero(angulo, 100);
	NotUsed = 0;
	strcpy(angulo, argv[0]);
	global_angulo = atof(angulo);
	printf("Angulo = %f ºC\n", global_angulo );
	//Se guarad el dato en el fichero que será consultado por el servidor
	fp = fopen ("temp_last_angle_data", "w+");
	fprintf(fp, "%f",  global_angulo);
	fclose(fp);
	printf("---> Dato guardo en fichero 'temp_last_angle'\n");
	return 0;
}
/*!
   \brief Obtiene el angulo de la base de datos
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
 */
int get_angle(void)
{

	sqlite3 *db;
	static char *nombredb = "example.db";
	int rc;
	char sql[100];
	char *err_msg = 0;

	/** Se abre base de datos para obtener el último ángulo registrado*/
	rc = sqlite3_open(nombredb, &db);
	if ( rc ) {
		fprintf(stderr, COLOR_RED "---> No se puede abrir la base de datos: %s\n" COLOR_RESET, sqlite3_errmsg(db));
		return 0;
	} else {

		printf("\n<==========================================================>\n");
		timestamp(PRINT_TIME);
		printf("---> Base de datos %s abierta.\n", nombredb);
	}
	/** Buscamos el ultimo dato de inclinacion*/
	sprintf(sql, "SELECT Inclinacion FROM angulo WHERE Ind = (SELECT MAX(Ind) from angulo);");
	rc = sqlite3_exec(db, sql, callback_angulo, 0, &err_msg);
	/** Se cierra la base de datos*/
	sqlite3_close(db);
	timestamp(PRINT_TIME);
	printf("---> Base de datos %s cerrada.\n", nombredb);
	printf("<==========================================================>\n\n");
	return 0;

}
int tcp_cmd_captura( int newsockfd, char *buffer)
{
	int result;
	char resposta[BUFFSIZE];

	bzero(resposta, BUFFSIZE);
	char web[2000];
	bzero(web, 2000);

	const char *httpheader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
	TCPDEBUG("Found LOCAL URL webservice:[%s]\n", "captura.html" );
	//get_angle();
	webfile_read("grafico.html", web);

	char htmlweb[BUFFSIZE];
	bzero(htmlweb, BUFFSIZE);
	sprintf(htmlweb, web, global_angulo);
	sprintf(resposta, "%sContent-Length: %d\r\n\r\n%s", httpheader, (int)strlen(htmlweb), htmlweb);

	result = write(newsockfd, resposta, strlen(resposta));
	//TCPDEBUG("SENDED:[%s] ", resposta );

	return result;
}

/*!
   \brief Funcion que devuelve el dato del ultimo angulo extraido de la base de datos para la peticion AJAX
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
*/
int tcp_cmd_angleupdate( int newsockfd, char *buffer)
{
	int result;
	char resposta[BUFFSIZE];

	bzero(resposta, BUFFSIZE);
	char web[2000];
	bzero(web, 2000);

	const char *httpheader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
	TCPDEBUG("Found LOCAL URL webservice:[%s]\n", "temp_last_angle_data" );
	get_angle();
	webfile_read("temp_last_angle_data", web);

	char htmlweb[BUFFSIZE];
	bzero(htmlweb, BUFFSIZE);
	sprintf(htmlweb, web, global_angulo);
	sprintf(resposta, "%sContent-Length: %d\r\n\r\n%s", httpheader, (int)strlen(htmlweb), htmlweb);

	result = write(newsockfd, resposta, strlen(resposta));
	//TCPDEBUG("SENDED:[%s] ", resposta );

	return result;
}

int tcp_cmd_error( int newsockfd, char *buffer )
{
	int result;

	const char *error_http_msg = "HTTP/1.1 404 Not Found \r\n\
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

	bzero(buffer, BUFFSIZE);

	sprintf(buffer, "%s", error_http_msg );

	result = write(newsockfd, buffer, strlen(buffer));
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
	int optval;  /* flag value for setsockopt */

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		tcp_error("ERROR opening socket");
	}

	/* setsockopt: Handy debugging trick that lets
	 * us rerun the server immediately after we kill it;
	 * otherwise we have to wait about 20 secs.
	 * Eliminates "ERROR on binding: Address already in use" error.
	 */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
		   (const void*)&optval, sizeof(int));

	bzero((char*)&serv_addr, sizeof(serv_addr));
	//portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *)&serv_addr,
		 sizeof(serv_addr)) < 0) {
		tcp_error("ERROR on binding");
	}
	listen(sockfd, 5);

	return sockfd;
}

int tcp_close(int sockfd)
{
	//TCPDEBUG( "close\n" );
	close(sockfd);
	return 0;
}


int tcp_service( int sockfd )
{
	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	char buffer[BUFFSIZE];
	int n, result;

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
			   (struct sockaddr *)&cli_addr,
			   &clilen);

	if (newsockfd < 0) {
		tcp_error("ERROR on accept");
		return -1;
	}

	bzero(buffer, BUFFSIZE);
	result = read(newsockfd, buffer, BUFFSIZE - 1);
	if (result < 0) {
		tcp_error("ERROR reading from socket");
	} else{
		TCPDEBUG("READED:[%s]\n", buffer );
	}

	// - Check URL
	if ( strstr( buffer, "GET /captura.html" ) ) {
		// buidar l'array aquí i generar la cadena buffer de json
		TCPDEBUG("INC\n");
		result = tcp_cmd_captura( newsockfd, buffer);
	}else if ( strstr( buffer, "GET /temp_last_angle_data" ) ){
		// buidar l'array aquí i generar la cadena buffer de json
		TCPDEBUG("INC\n");
		result = tcp_cmd_angleupdate( newsockfd, buffer);
	}else {
		result = tcp_cmd_error( newsockfd, buffer );
	}

	if (result < 0) {
		tcp_error("ERROR writing to socket");
	}
	close(newsockfd);

	return 0;

}
/*!
   \brief Lee del fichero la pagina web a mostrar y la deja en str
   \param "char *fichero: Nombre del fichero de la pagina web"
   \param "char *str: Variable donde se guardará el texto de la web"
   \return "False si OK, -1 si NOK"
 */
int webfile_read(char *fichero, char *str)
{
	FILE *fp;
	//char str[2000];        /** Variable donde se guardara el texto del fichero*/
	int i = 0;

	/* opening file for reading */
	fp = fopen(fichero, "r");
	if (fp == NULL) {
		perror("Error opening file");
		return -1;
	}else{
		printf("---> Fichero '%s' abierto con exito.\n",fichero);
	}
	/** se obtiene caracter a caracter el texto del fichero hasta encontrar el end of file*/
	while (!feof(fp)) {
		str[i] = fgetc(fp);
		i++;
	}
	/*Se elimina el último simbolo leido del fichero (da caracter desconocido. Interrongante en rombo)*/
	str[i-1] = '\0';
	printf("---> Contenio de '%s':\n", fichero);
	printf("%s\n", str);
	printf("---> fin de contenido de '%s'\n", fichero);
	fclose(fp);

	return 0;
}

int main(int argc, char *argv[])
{
	int sockfd = 0;
	int result = 0;

	if (argc < 2) {
		printf("Usage: webserver port\n");
		return -1;
	}
	sockfd = tcp_open(atoi(argv[1]));
	int num_connection = 1;

	do {
		timestamp(PRINT_TIME);
		TCPDEBUG(" - tcp_service(%03d): Starts...\n", num_connection);
		result = tcp_service(sockfd);
		timestamp(PRINT_TIME);
		TCPDEBUG(" - tcp_service(%03d): ...Ends\n", num_connection);
		TCPDEBUG("\n\n=============================================\n\n");

		num_connection++;
	} while (result == 0);

	tcp_close(sockfd);
}
