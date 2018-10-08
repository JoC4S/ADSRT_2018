/*!
   \file "main.c"
   \brief "Programa ADST_2018_1104934"
   \author "1104934"
   \date "23"/"10"/"2018"
 * Copyright (C) 2012- 2018
 *
 * Version   1.X
 * License GNU/GPL, see COPYING
 * This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.
      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.
      You should have received a copy of the GNU General Public License
      along with this program.  If not, see http://www.gnu.org/licenses/.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sqlite3.h>
#include <string.h>


#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\031[43m"
#define COLOR_RESET "\033[0m"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
//#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>

/*!
   \brief "Obtencion de fecha con formato sqlite DATETIME"
   \param "No tiene parametros"
   \return "Devuelve puntero a un string con la fecha"
 */
char* timestamp(void)
{
	time_t rawtime;
	struct tm *info;
	char buffer[80];
	char *retp;

	time( &rawtime );

	info = localtime( &rawtime );
	//El valor de retorn es a una variable de tipus timei_t, on posaràl temps en segons des de 1970-01-01 00:00:00 +0000 (UTC)

	// struct tm {
	//     int tm_sec;         /* seconds */
	//     int tm_min;         /* minutes */
	//     int tm_hour;        /* hours */
	//     int tm_mday;        /* day of the month */
	//     int tm_mon;         /* month */
	//     int tm_year;        /* year */
	//     int tm_wday;        /* day of the week */
	//     int tm_yday;        /* day in the year */
	//     int tm_isdst;       /* daylight saving time */
	//};

	strftime(buffer,80,"%Y-%m-%d %H:%M:%S", info);
	//printf("Formatted date & time : %s\n", buffer );
	retp = buffer;

	return retp;
}

/*!
   \brief "Callback de la funcion sqlitefunc"
   \return "DEvuelve los resultados del Query"
 */
static int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;

	fprintf(stderr, "%s: ", (const char*)data);

	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
/*!
   \brief "Funcion para uso del sqlite"
   \param "Pasamos opcion y nombre de la base de datos."
   \return "Devuelve integer"
 */
int dbfunc(char *opcion, char *nombredb)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *zErrMsg = 0;
	int rc;
	/** pasamos el parametro de opcion a integer*/
	int c = atoi(opcion);
	char *sql = NULL;
	const char* data = "Callback function called\n\n";
	char horamuestreo[20];
	float mustratemperatura = 18.3;
	int estadoventilador = 1;

	strcpy(horamuestreo, timestamp());

	/* Abrimos la base de datos. */
	rc = sqlite3_open(nombredb, &db);
	if ( rc ) {
		fprintf(stderr, "---> No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
		return 0;
	} else {
		if (nombredb == NULL) {
			fprintf(stderr, " ERROR: Se necesita un nombre para la base de datos.\n");
			return 1;
		}
		else
			printf("---> Base de datos abierta con exito.\n");
	}
	/* Create merged SQL statement */
	/** Ejecutaremos los SQL statmenst en funcion de la opcion que le pasemos a la funcion*/
	switch (c) {

	case 1: /** Crear Tablas en la base de datos*/

		/** Tabla de DATOS para datos capturados e información del estado del ventilador*/
		sql = "CREATE TABLE DATOS (HORA DATETIME  NOT NULL,TEMPERATURA FLOAT NOT NULL, VENTILADOR INT NOT NULL);" \
		      "CREATE TABLE ALARMAS (HORA DATETIME  NOT NULL, TEMP_FAN_ON INT NOT NULL);";
		printf(" |--->Crear tabla 'DATOS'.\n");
		printf(" |--->Crear tabla 'ALARMAS'.\n");

		break;
	case 2: /** Insertar datos en la tabla de 'DATOS'*/
		/** Utilizamos 'sqlite3_prepare' para insertar los datos a partir de variables.*/
		/** ?1 : Hora del sistema a la que se registra la muestra*/
		/** ?2: Muestra del dato de temperatura registrado*/
		/** ?3: Estado del Ventilador. 0: Apagado; 1: Puesta en Marcha; 2: Mantener encedido*/
		sqlite3_prepare_v2(db, "insert into DATOS (HORA, TEMPERATURA, VENTILADOR) values (?1, ?2, ?3);", -1, &stmt, NULL);       /* 1 */
		sqlite3_bind_text(stmt, 1, horamuestreo, -1, SQLITE_STATIC);                                             /* 2 */
		sqlite3_bind_double(stmt, 2, mustratemperatura);
		sqlite3_bind_int(stmt, 3, 1);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
		}
		else {
			printf("\nIntroducidos en tabla 'DATOS:'\n" );
			printf("----> Timestamp: %s'\n", horamuestreo );
			printf("----> Temperatura: %2.2f ºC'\n",mustratemperatura);
			printf("----> Estado Ventilador: %d'\n\n",estadoventilador);
		}
		sqlite3_finalize(stmt);
		break;
	case 3: /** Consultar datos de la tabla*/
		sql = "SELECT * from DATOS";
	default:
		printf("Ninguna acción a realizar con la base de datos.\n");
	}

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

	if ( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	return 0;
}

/*!
   \brief "Configura y abre el puerto serie"
   \param "Param description"
   \return "Return of the function"
 */

// Variables para configuracion puerto Serie y la fucnion de commSerie
#define BAUDRATE B9600
//#define MODEMDEVICE "/dev/ttyS0"       //Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"      // Conexió directa PC(Linux) - Arduino
#define _POSIX_SOURCE 1                 // POSIX compliant source

struct termios oldtio, newtio;
int fd = 0;

int ConfigurarSerie(int fd)
{

	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror(MODEMDEVICE);
		fprintf(stderr,"No se ha podido abrir el puerto '/dev/ttyACM0'.\n");
		exit(-1);
	}
	else
		printf("Puerto Serie abierto: '/dev/ttyACM0'\n");
	tcgetattr(fd, &oldtio); /* save current port settings */
	bzero(&newtio, sizeof(newtio));
	// newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1;  /* blocking read until 1 chars received */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
	// sleep(1); //Per donar temps a que l'Arduino es recuperi del RESET
	return fd;
}
/*!
   \brief "Cierra el puerto Serie"
   \param "int fd: File descriptor"
   \return "void"
*/
void TancarSerie(int fd)
{
	//tcsetattr(fd, TCSANOW, &oldtio);
	if (close(fd) != 0){
		fprintf(stderr, "No se ha podido cerrar el puerto: '/dev/ttyACM0'.\n");
	}else{
		printf("Puerto serie cerrado: '/dev/ttyACM0'.\n");
	}
}
/*!
   \brief "Enviar mensage por puerto serie : SendcommSerie(int fd, char *missatgeSerie)."
   \param "int fd: Descripción de puerto serie ; char *missatgeSerie : Mensage a enviar."
   \return "Integer."
 */
int SendcommSerie(int fd, char *missatgeSerie)
{
	int i = 0;
	int res = 0;
	// Bloqueamos acceso al puerto Serie al resto de threads y procesos.
	//pthread_mutex_lock(&varmutex);
	// Enviar el missatge 1
	res = write(fd, missatgeSerie, strlen(missatgeSerie));
	if (res < 0) {
		tcsetattr(fd, TCSANOW, &oldtio);
		perror(MODEMDEVICE);
		exit(-1);
	}
	// Mostramos por pantalla el mensaje y los bytes enviados.
	printf(COLOR_GREEN "<--Enviats Serie %d bytes.\n", res );
	printf (COLOR_RESET );
	return 0;
}
/*!
   \brief "Funcion para la recepcion por Serie : recieveCommSerie(int fd, char buf)""
   \param "int fd; char buf"
   \return "Return of the function"
*/
/*!< char buf[256] : Variable global para el almacenamiento de los mensajes recibidos por puerto Serie. */
char buf[256];

int recieveCommSerie(int fd, char buf[256])
{
	int i = 0;
	int errorMsg = 0;
	int bytes = 0;
	printf("emtra en la funcion receiveCommSerie\n");

	/** Se borra el buffer Serie antes de iniciar la recepción.*/
	memset(buf, '\0', 256);
	/**Leemos el buffer de entrada de uno en uno hasta tener un mensaje completo.*/
	while (!bytes) {
		ioctl(fd, FIONREAD, &bytes);
		while (bytes > 0) {
			printf("dentro del while %d\n",i);
			read(fd, buf + i, 1);
			bytes--;
			i++;
		}
		if (buf[0] == 'A' && (buf[3] == 'Z' || buf[7] == 'Z')) {
			printf(COLOR_GREEN "Rebut Serie %ld bytes: %s", strlen(buf), buf);
			break;
		}
	}
	printf(COLOR_RESET "\n");
	// Se libera el puerto Serie
	//pthread_mutex_unlock(&varmutex);
	return errorMsg;
}

/*!
   \brief "Funcion Principal"
   \param "Le pasamos patats, tomates y cebolla"
   \return "Nos devuelve un caldo"
 */
int main(int argc, char *argv[])
{
	char *nombredb = NULL;                           /** Valor del parameto o*/
	char *ovlaue = NULL;                             /** Valor del parameto n*/
	int index;
	int c;

	while ((c = getopt(argc, argv, "n:o:h")) != -1) {
		switch (c) {
		case 'n':
			nombredb = optarg;
			break;
		case 'o':
			ovlaue = optarg;
			break;
		case 'h':
			printf("\nUso: main [opciones] archivo...\n");
			printf("\n-n	Nombre base de datos. Precisa 'nombre_base_datos.db'\n");
			printf("-o	Accion a realizar: 1 -Crear db. 2- Llenar Tabala.\n\n");
			return 0;
		case '?':
			if (optopt == 'n' || optopt == 'o') {
				printf("\nLa opcion -%c requiere un argumento.\n", optopt);
			} else{
				printf("Opcion desconocida  '%c'.\n", optopt);
			}
			printf("Usar '-h' para Ayuda.\n");
			return 1;
		default:
			abort();
		}
	}

	for (index = optind; index < argc; index++) {
		printf("Non-option argument %s\n", argv[index]);
	}
	printf("\nNombre base de datos: %s\n", nombredb);
	if (optind == 1) {
		printf("Se necesitan parametros. Usar -h para Ayuda.\n");
		return 1;
	}
	dbfunc(ovlaue, nombredb);
	/** Se configura y abre el puerto serie*/
	ConfigurarSerie(fd);

	/** Enviamos el parametro para que se inicie la adquisicion de datos.*/
	/**Operació 'M'
	: Marxa / Parada conversió
	* COMANDA: 'A''M' v Temps Temps Núm 'Z'
		v: 	(0=parada / 1=marxa)
		Temps: 	temps en segons de mostreig (1..20)
		Núm: 	Número de mostres fer la mitjana (1..9)	*/
	SendcommSerie(fd, "AM1053Z");
	/** Esperamos la respuesta del Arduino al mensaje de inicio de adquisicion.*/
	recieveCommSerie(fd, buf);
	if (buf[2] != '0'){
		fprintf(stderr, "ERROR de protocolo.\n");
		return 1;
	}
	TancarSerie(fd);

	return 0;
}
