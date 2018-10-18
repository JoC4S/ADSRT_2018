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
#include <sys/signal.h>
#include <time.h>
#include <sqlite3.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
//#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>

#define COLOR_GREEN "\x1B[32m"
#define COLOR_RED "\x1B[31m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_RESET "\033[0m"
#define PIN_VENTILADOR "03"                     /** Pin de Arduino que activa el ventilador*/
#define CONST_TEMP 0.04883                       /*!< Constante de ttemperatura para el sensor mV * nivel. */
#define TIMEOUT_TEMP 3                          /** Tiempo en segundos para provocar un error de timeout*/
#define TIME_ADQ 5
#define FAN_ON "1"
#define FAN_OFF "0"
#define FAN_STATUS "2"
#define CREATE_DB 1				/** Para función dbfunc(). Indica la solicitud  de creacion de la base de datos-*/
#define CONSINA_TEMP 15.0
#define PRINT_TIME 1				/** Para funcion timestamp(). Idica que se imprimira la hora por patnalla.*/
#define ADQUISICION_DATOS_PERIODICA 2

float ultimaTemperatura;                        /** valor de la ultima muestra de temperatura adquirida*/
int alarmTemp;                                  /** Numero de veces en que no se ha conseguido controlar la temperatura*/
char *nombredb = NULL;                          /** Valor del parameto o*/

#define INTERVAL 3000                           /* number of milliseconds to go off */

// Variables para configuracion puerto Serie y la fucnion de commSerie
#define BAUDRATE B9600                          /** Velocidad de transferecnia en comunicacion serie.*/
//#define MODEMDEVICE "/dev/ttyS0"              //Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"              // Conexió directa PC(Linux) - Arduino
struct termios oldtio, newtio;

int fd = 0;                                     //File Descriptor para el puerto Serie
/*!< char buf[256] : Variable global para el almacenamiento de los mensajes recibidos por puerto Serie. */
char buf[256];

/** Declaración de funciones*/
int fanOnOff (char *estado);
float temperatura ();

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
	if (print)
		printf(COLOR_YELLOW "%s" COLOR_RESET, buffer );
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
   \brief "Comprueba que la temperatura este siendo controlada."
   \brief "Se le pasa el ultimo dato de temperatura. En caso de llevar 5 tomas de temperatura"
   \brief "seguidas sobrepasando la consigna, se realiza una inserción de alarma en la tabal de alarmas sqlite"
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
 */
int controlTemp (float ultimatemperatura, int fanstatus){

	float consignaTemp = 20.0;

	if (ultimatemperatura > consignaTemp) {
		if (fanstatus) {
			printf("El ventildaor ya esta encendido!\n");
			alarmTemp++;
		}else{
			fanOnOff("1");
			printf("|-----> Se activa el ventilador.\n" );
		}
	}else{
		alarmTemp = 0;
		if (fanstatus)
			fanOnOff("0");
	}
	if (alarmTemp == 5) {
		/* Introducir alarma en sql*/
	}

}
/*!
   \brief "Funcion para uso del sqlite"
   \param "Pasamos opcion y nombre de la base de datos."
   \param "Opción = 0 : Abrir base de datos"
   \param "Opción = 1 : ACrear tablas de DATOS y ALARMA"
   \param "Opción = 3 : Insertar datos adquiridos."
   \return "Devuelve integer"
 */
int dbfunc(int opcion, char *nombredb)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *zErrMsg = 0;
	int rc;
	int c = opcion;
	float consignaTemp = 15.0;
	char *sql = NULL;
	const char* data = "Callback function called\n\n";
	char horamuestreo[20];
	float mustratemperatura;
	int fanstatus;

	/** Abrimos la base de datos.*/
	rc = sqlite3_open(nombredb, &db);
	if ( rc ) {
		fprintf(stderr,COLOR_RED "---> No se puede abrir la base de datos: %s\n" COLOR_RESET, sqlite3_errmsg(db));
		return 0;
	} else {
		if (nombredb == NULL) {
			fprintf(stderr,COLOR_RED " ERROR: Se necesita un nombre para la base de datos.\n" COLOR_RESET);
			return 1;
		}
		else
			timestamp(PRINT_TIME);
			printf("---> Base de datos abierta con exito.\n");
	}
	switch (c) {

	case 1: /** Tabla de DATOS para datos capturados e información del estado del ventilador*/
		//strcpy(sql,"CREATE TABLE DATOS (HORA DATETIME  NOT NULL,TEMPERATURA FLOAT NOT NULL, VENTILADOR INT NOT NULL);");
		//strcpy(sql,"CREATE TABLE ALARMAS (HORA DATETIME  NOT NULL, TEMP_FAN_ON INT NOT NULL);");
		sql = "CREATE TABLE DATOS (HORA DATETIME  NOT NULL,TEMPERATURA FLOAT NOT NULL, VENTILADOR INT NOT NULL);" \
		      "CREATE TABLE ALARMAS (HORA DATETIME  NOT NULL, TEMP_FAN_ON INT NOT NULL);";
		printf(" |--->Crear tabla 'DATOS'.\n");
		printf(" |--->Crear tabla 'ALARMAS'.\n");
		break;
	case 2:
		/** Insertar datos en la tabla de 'DATOS'*/
		/** Utilizamos 'sqlite3_prepare' para insertar los datos a partir de variables.*/
		/** ?1 : Hora del sistema a la que se registra la muestra*/
		/** ?2: Muestra del dato de temperatura registrado*/
		strcpy(horamuestreo,timestamp(0));
		mustratemperatura = temperatura();
		fanstatus = fanOnOff(FAN_STATUS);
		/** Comprobacion de alarma de TEMPERATURA*/
		if (mustratemperatura > consignaTemp) {
			if (fanstatus) {
				printf("El ventildaor ya esta encendido!\n");
				alarmTemp++;
				printf("Alarmas de temperatura: %d\n", alarmTemp);
			}else{
				fanOnOff(FAN_ON);
				printf("|-----> Se activa el ventilador.\n" );
			}
		}else{
			alarmTemp = 0;
			if (fanstatus)
				fanOnOff(FAN_OFF);
		}
		/** Prepara los datos comunes para las tablas DATOS y ALARMAS.*/

		/** En caso de llevar 5 veces seguidas con el ventilador encendido, se introducira el dato en ALARMA*/
		if (alarmTemp >= 3) {/* Preparar datos para tabla de ALARMA en sql*/
			sqlite3_prepare_v2(db, "insert into ALARMAS (HORA, TEMP_FAN_ON) values (?1, ?2);", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, horamuestreo, -1, SQLITE_STATIC);
			sqlite3_bind_int(stmt, 2, alarmTemp);
			/** Finalizamos la creación del statement para el sqlite y finalizamos cargandolo en la base de datos.*/
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE) {
				printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
			}
			else {
				printf("\nIntroducidos en tabla" COLOR_RED "'ALARMAS:'\n"COLOR_RESET );
				printf("----> Timestamp: %s'\n", horamuestreo );
				printf("----> Tiempo vent. encendido: %d min'\n",alarmTemp);
			}
			sqlite3_finalize(stmt);
		}
		/** Prepara los datos para rellenar la tabla DATOS*/
		sqlite3_prepare_v2(db, "insert into DATOS (HORA, TEMPERATURA, VENTILADOR) values (?1, ?2, ?3);", -1, &stmt, NULL);
		sqlite3_bind_text(stmt, 1, horamuestreo, -1, SQLITE_STATIC);
		sqlite3_bind_double(stmt, 2, mustratemperatura);
		sqlite3_bind_int(stmt, 3, fanstatus);
		/** Finalizamos la creación del statement para el sqlite y finalizamos cargandolo en la base de datos.*/
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
		}
		else {
			printf("\nIntroducidos en tabla 'DATOS:'\n" );
			printf("----> Timestamp: %s'\n", horamuestreo );
			printf("----> Temperatura: %2.2f ºC'\n",mustratemperatura);
			printf("----> Estado Ventilador: %d'\n",fanstatus);
		}
		sqlite3_finalize(stmt);
		break;
	case 3: /** Consultar datos de la tabla*/
		strcpy(sql,"SELECT * from DATOS");
		//sql = "SELECT * from DATOS";
		break;

	case 4:/** Insercion de datos en tabla de ALARMAS*/

		break;
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
	timestamp(PRINT_TIME);
	printf("---> Base de datos cerrada.\n\n");
	return 0;
}

/*!
   \brief "Configura y abre el puerto serie"
   \param "Param description"
   \return "Return of the function"
 */

int ConfigurarSerie(int fd)
{

	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror(MODEMDEVICE);
		fprintf(stderr,COLOR_RED "No se ha podido abrir el puerto '/dev/ttyACM0'.\n" COLOR_RESET);
		exit(-1);
	}
	else
		printf(COLOR_GREEN "Puerto Serie abierto: '/dev/ttyACM0'\n" COLOR_RESET);
	tcgetattr(fd, &oldtio); /* save current port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	//newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1;  /* blocking read until 1 chars received */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
	sleep(1); //Per donar temps a que l'Arduino es recuperi del RESET
	return fd;
}
/*!
   \brief "Cierra el puerto Serie"
   \param "int fd: File descriptor"
   \return "void"
 */
void TancarSerie(int fd)
{
	/** EL parametro TCSANOW implica que la accion se realiaz de forma inmediata*/
	tcsetattr(fd, TCSANOW, &oldtio);
	if (close(fd) != 0) {
		fprintf(stderr, "No se ha podido cerrar el puerto: '/dev/ttyACM0'.\n");
	}else{
		printf(COLOR_GREEN "\nPuerto serie cerrado: '/dev/ttyACM0'.\n");
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
	printf(COLOR_YELLOW "%s ",missatgeSerie );
	printf(COLOR_GREEN "--> :Envio Serie %d bytes.\n", res );
	printf (COLOR_RESET );
	return 0;
}

/*!
   \brief "Funcion para la recepcion por Serie.
   \brief En caso de error de comunicación se realizan 3 intentos.
   \param "int fd; char buf"
   \return "Return of the function"
 */
int recieveCommSerie(int fd, char buf[256])
{
	int i = 0, bytes = 0;
	int receiveState = 0;
	int c = 0;
	/** conf SELECT*/
	fd_set rfds;
	struct timeval tv;
	int retval;
	// Se borra el buffer Serie antes de iniciar la recepción.
	memset(buf, '\0', 256);
	tcflush(fd, TCIFLUSH);

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	/* Wait up to five seconds. */
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	printf("Esperando respuesta...\n" );
	retval = select(fd+1, &rfds, NULL, NULL, &tv);
	/* Don’t rely on the value of tv now! */
	if (retval == -1)
		perror("select()");
	else if (retval) {
		/** Data is available now*/
		if (FD_ISSET(fd, &rfds)) {               /* will be true. */
			while (!bytes) {
				usleep(100000);
				ioctl(fd, FIONREAD, &bytes);
				while (bytes > 0) {
					read(fd, buf + i, 1);
					bytes--;
					i++;
				}
				/** Si tenemos ocurre un timeout, sa sale de la función, dando error*/
				if (buf[0] == 'A' && (buf[3] == 'Z' || buf[4] == 'Z' || buf[7] == 'Z')) {
					printf(COLOR_YELLOW "%s ", buf);
					printf(COLOR_GREEN "<-- :Recibido Serie %ld bytes.\n" COLOR_RESET, strlen(buf));
					receiveState = 0;
					break;
				}
			}
		}
	}else{
		printf(COLOR_RED"No data within %d seconds.\n" COLOR_RESET,TIMEOUT_TEMP);
	}
	tcflush(fd,TCIOFLUSH);
	return receiveState;
}

/*!
   \brief Menú para el debug del programa.
   \param "Param description"
   \return "Return of the function"
 */
int menudebug(void)
{

	int opcion = 0;
	printf(COLOR_YELLOW  "-------------------------\n");
	printf("Opciones:\n");
	printf("-------------------------\n");
	printf("1- Solicitar temperatura.\n");
	printf("2- Encender Ventilador.\n");
	printf("3- Apagar Ventilador.\n");
	printf("4- Solicitar estado de Ventilador.\n");
	printf("5- Solicitar temp y fan Stat y rellenar db.\n");
	printf("6- Forzar comunicacion no contesta.\n");
	printf("7- Detener Programa.\n");
	printf("-------------------------\n\n" COLOR_RESET);
	printf("Seleccionada opción: ");
	scanf("%d",&opcion );

	return opcion;
}

/*!
   \brief "Enciende/Apaga el Ventilador o solicita informacion de su estado."
   \param "char * estado: 1- Encender, 0- Apagar, 2 - Solicitar estado"
   \pre "PIN_VENTILADOR comprendido entre 0 y 13."
   \return "int OnOff: 1= Ventilador encendido, 0=Ventilador apagado."
 */
int fanOnOff (char *fan)
{
	char orden[7] = "\0";
	int OnOff;
	/** Activa o desactiva el ventilador*/
	if ((fan == "0") || (fan == "1")) {
		strcpy(orden, "AS");
		strcat(orden, PIN_VENTILADOR);
		strcat(orden,fan);
		strcat(orden,"Z");
		SendcommSerie(fd, orden);
		recieveCommSerie(fd,buf);
		OnOff = buf[2];
		if (buf[1] != 'S' || buf[2] == '2')
			fprintf(stderr,COLOR_RED "Error en recepción\n" COLOR_RESET );
		/** Se borra el ultimo msg recibido y el buffer Serie de entrada*/
		memset(buf, '\0', 256);
		tcflush(fd,TCIOFLUSH);
		OnOff = atoi(fan);
	}
	/** Solicita el estado del ventilador*/
	if (fan == "2") {
		strcpy(orden, "AE");
		strcat(orden, PIN_VENTILADOR);
		strcat(orden,"Z");
		SendcommSerie(fd, orden);
		recieveCommSerie(fd,buf);
		OnOff = (buf[3] -48);
		if (OnOff)
			printf("|---> Ventilador ON.\n" );
		else
			printf("|---> Ventilador OFF.\n");
		/** Se borra el ultimo msg recibido y el buffer Serie de entrada*/
		memset(buf, '\0', 256);
		tcflush(fd,TCIOFLUSH);
	}
	return OnOff;
}

/*!
   \brief "Obtiene el dato de temperatura del sensor."
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "float muestratemperatura"
 */
float temperatura ()
{
	char orden[7] = "\0";
	int i;
	char tmp[6] = "\0";
	int muestratemperatura_raw = 0;
	float muestratemperatura = 0.0;
	strcpy(orden, "ACZ");
	SendcommSerie(fd, orden);
	recieveCommSerie(fd,buf);
	/** Se convierte el dato del buffer, se pasa a integer
	   y se multiplica por la constante del sensor para obtener la temperatura*/
	for (i = 0; i < 5; i++) {
		tmp[i] = buf[i+3];
	}
	if (buf[1] != 'C' || buf[2] != '0') {

		fprintf(stderr, "Error en recepción\n" );
		return -1;
	}else {
		/** Se borra el ultimo msg recibido y el buffer Serie de entrada*/
		memset(buf, '\0', 256);
		tcflush(fd,TCIOFLUSH);
		/** Se prepara el dato de temperatura para mostrarlo por pantalla y devolverlo*/
		muestratemperatura_raw = atoi(tmp);
		muestratemperatura = muestratemperatura_raw * CONST_TEMP;
		printf("|---> Temperatura: %2.2f\n",muestratemperatura);
		return muestratemperatura;
	}
}
/*!
   \brief "Adquiere de forma periodica los datos del sensor de temperatura y los almacena en la base de datos."
   \param "int fd: Descripcion de fichero para puerto Serie"
   \param "char * accion: accion a ejecutar"
   \param "	1 - Solicitud de dato de temperatura."
   \param "	2 - Encender Ventilador."
   \param "	3 - Apagar Ventilador."
   \param "	4 - Solicitar estado de Ventilador."
   \param "	5 - Solicitud de dato y almacenamiento en sqlite."
   \param "char * debug: Habilita el menu de Debug"
   \pre "El peurto de comunicación serie debe estar abierto."
   \pre "La base de datos debe estar previamente creada."
   \return "Return of the function"
 */
int adquisicion (int fd, char * accion, int debug, char buf[256])
{
	int i = 0;
	char orden[7] = "\0";
	int bucle = 0;
	while (!bucle) {
		if (debug) {
			i = menudebug();
		} else
			i = atoi(accion);
		switch (i) {
		case 1:         /** Solicitar TEMPERATURA*/
			temperatura();
			break;
		case 2:         /** Enciede el VENTILADOR ASnnvZ */
			fanOnOff("1");
			break;
		case 3:         /** Apaga el VENTILADOR ASnnvZ */
			fanOnOff("0");
			break;
		case 4:         /** Solicita estado del ventilador AEnnZ*/
			fanOnOff("2");
			break;
		case 5:         /** LLena la base de sqlite con la info de temperatura solicitada */
			dbfunc(2, nombredb);
			break;
		case 6:         /** Solicita un cambio en el Searial.readtimeout de Arduino*/
			        /** Esto provoca el fallo de Serial tiemout al no responder*/
			        /** Arduino antes de 3 segundos.*/
			        /** Si se vuelve a solicitar el comando, deja el valor como estaba.*/
			strcpy(orden, "AT");
			strcat(orden,"Z");
			SendcommSerie(fd, orden);
			recieveCommSerie(fd,buf);
			break;
		case 7:         /** Salir del programa*/
			printf("Saliendo de programa.\n");
			bucle = 1;
			break;
		}
	}
	return 1;
}

/*!
   \brief "Funcion Principal"
   \param ""
   \return ""
 */
int main(int argc, char *argv[])
{
	int dvalue = 0;
	char *accion = NULL;                           /** Valor del parameto -d para debug del programa*/
	char tvalue = TIME_ADQ;                         /** Tiempo de adquisicion de muestras por defecto*/
	int index;
	int c;

	/** ADquisición de los parametros de lanzamiento del programa*/
	/** Pendiente agregar opción para la adquisición de la temperatura objetivo a mantener*/
	while ((c = getopt(argc, argv, "n:t:hd")) != -1) {
		switch (c) {
		case 'n':
			nombredb = optarg;
			break;
		case 'd':
			dvalue = 1;
			break;
		case 't':
			tvalue = atoi(optarg);
			break;
		case 'h':
			printf("\nUso: main [opciones] -n [nombre_archivo.db]...\n\n");
			printf("-n [nombre.db]	Nombre base de datos. Precisa 'nombre_base_datos.db'\n");
			printf("-t [seg]	Tiempo de muestreo en sgundos. Default = 5s.\n\n");
			printf("-m [int]	Nº Muestras para hacer la media. Default = 3.\n\n");
			printf("-d              Modo Debug. Para menu de opciones.\n\n");
			exit(1);
		case '?':
			if (optopt == 'n' ) {
				printf("\nLa opcion -%c requiere un argumento.\n", optopt);
			} else{
				printf("Opcion desconocida  '%c'.\n", optopt);
			}
			printf("Usar '-h' para Ayuda.\n");
			break;
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
		exit(1);
	}
	/** Se configura y abre el puerto serie*/
	fd = ConfigurarSerie(fd);
	/** Abrir la base de datos con el nombre suministrado y crear las tablas*/
	dbfunc(CREATE_DB, nombredb);
	/** Enviamos el parametro para que se inicie la adquisicion de datos.*/
	/**Operació 'M'
	   : Marxa / Parada conversió
	 * COMANDA: 'A''M' v Temps Temps Núm 'Z'
	        v:      (0=parada / 1=marxa)
	        Temps:  temps en segons de mostreig (1..20)
	        Núm:    Número de mostres fer la mitjana (1..9)	*/
	printf("Arrancando proceso de toma de temperatura en Arduino.\n");
	char arranque [] = "AM1013Z";
	SendcommSerie(fd, arranque);
	/** Esperamos la respuesta del Arduino al mensaje de inicio de adquisicion.*/
	recieveCommSerie(fd, buf);

	/** En caso de no haber error, iniciamos la adquisicion de datos*/
	/** si la opcion de debug está activada, sacara el menu por pantalla*/
	/*Si no hay modo debug, solicitara muestras cada tvalue */
	if (dvalue) {
		printf("\nEntrando en modo Debug....\n" );
		adquisicion(fd, accion, dvalue, buf);
	}else{
		c = 0;
		while(!c) {
			c = dbfunc(ADQUISICION_DATOS_PERIODICA, nombredb);
			sleep(tvalue);
		}
	}
	/** Apagamos ciclo de recoleccion de datos en Arduino.*/
	strcpy(arranque, "AM0013Z");
	SendcommSerie(fd, arranque);
	/** Esperamos la respuesta del Arduino al mensaje de inicio de adquisicion.*/
	recieveCommSerie(fd, buf);
	/** Cerramos puerto Serie.*/
	TancarSerie(fd);
	return 0;
}
