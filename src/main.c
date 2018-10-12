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
#define PIN_VENTILADOR "03"     		/** Pin de Arduino que activa el ventilador*/
#define CONST_TEMP 0.048        		/*!< Constante de ttemperatura para el sensor mV * nivel. */
#define TIMEOUT_TEMP 3				/** Tiempo en segundos para provocar un error de timeout*/
int SerialCommTimeOUT;				/** Flag de timeout*/
int AlarmCommTimeOUT;				/** Flag de Alarma de tiemout activada*/
char *nombredb = NULL;          		/** Valor del parameto o*/

// Variables para configuracion puerto Serie y la fucnion de commSerie
#define BAUDRATE B9600				/** Velocidad de transferecnia en comunicacion serie.*/
//#define MODEMDEVICE "/dev/ttyS0"       	//Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"      	// Conexió directa PC(Linux) - Arduino

int fd = 0;                             	//File Descriptor para el puerto Serie

/** Declaración de funciones*/
int fanOnOff (char *estado);
float temperatura ();

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
   \param "Opción = 0 : Abrir base de datos"
   \param "Opción = 1 : ACrear tablas de DATOS y ALARMA"
   \param "Opción = 3 : Insertar datos adquiridos."
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
			printf("---> Base de datos abierta con exito.\n");
	}
	switch (c) {

	case 1: /** Tabla de DATOS para datos capturados e información del estado del ventilador*/
		sql = "CREATE TABLE DATOS (HORA DATETIME  NOT NULL,TEMPERATURA FLOAT NOT NULL, VENTILADOR INT NOT NULL);" \
		      "CREATE TABLE ALARMAS (HORA DATETIME  NOT NULL, TEMP_FAN_ON INT NOT NULL);";
		printf(" |--->Crear tabla 'DATOS'.\n");
		printf(" |--->Crear tabla 'ALARMAS'.\n");
		break;
	case 2: /** Insertar datos en la tabla de 'DATOS'*/
		/** Utilizamos 'sqlite3_prepare' para insertar los datos a partir de variables.*/
		/** ?1 : Hora del sistema a la que se registra la muestra*/
		/** ?2: Muestra del dato de temperatura registrado*/
		strcpy(horamuestreo,timestamp());
		mustratemperatura = temperatura();
		fanstatus = fanOnOff("2");
		sqlite3_prepare_v2(db, "insert into DATOS (HORA, TEMPERATURA, VENTILADOR) values (?1, ?2, ?3);", -1, &stmt, NULL);
		sqlite3_bind_text(stmt, 1, horamuestreo, -1, SQLITE_STATIC);
		sqlite3_bind_double(stmt, 2, mustratemperatura);
		sqlite3_bind_int(stmt, 3, fanstatus);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
		}
		else {
			printf("\nIntroducidos en tabla 'DATOS:'\n" );
			printf("----> Timestamp: %s'\n", horamuestreo );
			printf("----> Temperatura: %2.2f ºC'\n",mustratemperatura);
			printf("----> Estado Ventilador: %d'\n\n",fanstatus);
		}
		sqlite3_finalize(stmt);
		break;
	case 3: /** Consultar datos de la tabla*/
		strcpy(sql,"SELECT * from DATOS");
		//sql = "SELECT * from DATOS";
		break;

	case 4:
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
	printf("Base de datos cerrada.\n");
	return 0;
}

/*!
   \brief "Configura y abre el puerto serie"
   \param "Param description"
   \return "Return of the function"
 */



int ConfigurarSerie(int fd){
	struct termios oldtio, newtio;

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
   \brief "Funcion de trigger del flag alarma de tiempo de comunicacion Serie excedido."
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
 */
void alarm_SerialCommtimeout(int signum){

	if (AlarmCommTimeOUT) {
		SerialCommTimeOUT = 1;
		fprintf(stderr, COLOR_RED "\nSerial Comm TIMEOUT!!\n" COLOR_RESET );
	}
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
/*!< char buf[256] : Variable global para el almacenamiento de los mensajes recibidos por puerto Serie. */
char buf[256];
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
	int delay = 0;
	/** set up alarm handler */
	signal(SIGALRM, alarm_SerialCommtimeout);
	AlarmCommTimeOUT= 1;
	SerialCommTimeOUT = 0;

	useconds_t sleeptime = 60000; /** Tiempo en microsegundos*/
	// Se borra el buffer Serie antes de iniciar la recepción.
	memset(buf, '\0', 256);
	/** Configuracion de la Alarma*/
	alarm(TIMEOUT_TEMP);
	// Leemos el buffer de entrada de uno en uno hasta tener un mensaje completo.
	while (!bytes) {
		usleep(sleeptime);
		delay = delay + sleeptime;
		if (SerialCommTimeOUT) {
			receiveState = 0;
			break;
		}
		ioctl(fd, FIONREAD, &bytes);
		while (bytes > 0) {
			read(fd, buf + i, 1);
			bytes--;
			i++;
		}
		/** Si tenemos ocurre un timeout, sa sale de la función, dando error*/
		if (buf[0] == 'A' && (buf[3] == 'Z' || buf[4] == 'Z' || buf[7] == 'Z')) {
			AlarmCommTimeOUT = 0;
			printf(COLOR_YELLOW "\n%s ", buf);
			printf(COLOR_GREEN "<-- :Recibido Serie %ld bytes.\n", strlen(buf));
			printf(COLOR_RESET);
			receiveState = 1;
			break;
		}
		printf("\rEsperando mensaje entrante (delay = %i us)...",delay);
		fflush(stdout);
	}
	printf(COLOR_RESET "\n");
	return receiveState;
}
int menudebug(void){

	int opcion = 0;
	printf(COLOR_YELLOW  "\n-------------------------\n");
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
   \return "int OnOff: 1= Ventilador encendido, 2=Ventilador apagado."
 */
int fanOnOff (char *fan){

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
float temperatura (){

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
   \param "Param description"
   \pre "El peurto de comunicación serie debe estar abierto."
   \pre "La base de datos debe estar previamente creada."
   \return "Return of the function"
 */

int adquisicion (int fd, char buf[256]){

	int i;
	char orden[7] = "\0";
	while (1) {
		i = menudebug();
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
			dbfunc("2", nombredb);
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
			TancarSerie(fd);
			exit(1);
		}
	}
}
/*!
   \brief Alarma Timeout comunicacion.
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
 */
void alarm_handler(int signum){
	printf("Comunicación timeout\n");
	SerialCommTimeOUT = 1;
}

/*!
   \brief "Funcion Principal"
   \param "Le pasamos patats, tomates y cebolla"
   \return "Nos devuelve un caldo"
 */
int main(int argc, char *argv[])
{
	char *dvlaue = NULL;                             /** Valor del parameto n*/
	int index;
	int c;

	/** Se configura y abre el puerto serie*/
	fd = ConfigurarSerie(fd);

	while ((c = getopt(argc, argv, "n:hd")) != -1) {
		switch (c) {
		case 'n':
			nombredb = optarg;
			break;
		case 'd':
			dvlaue = optarg;
			break;
		case 'h':
			printf("\nUso: main [opciones] archivo...\n");
			printf("\n-n	Nombre base de datos. Precisa 'nombre_base_datos.db'\n");
			printf("-t	Tiempo de muestreo. Default = 1s.\n\n");
			printf("-m	Nº Muestras para hacer la media. Default = 3.\n\n");
			printf("-d	Modo Debug. Para menu de opciones.\n\n");
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
	/** Abrir la base de datos con el nombre suministrado y crear las tablas*/
	dbfunc("1", nombredb);
	/** Enviamos el parametro para que se inicie la adquisicion de datos.*/
	/**Operació 'M'
	   : Marxa / Parada conversió
	 * COMANDA: 'A''M' v Temps Temps Núm 'Z'
	        v:      (0=parada / 1=marxa)
	        Temps:  temps en segons de mostreig (1..20)
	        Núm:    Número de mostres fer la mitjana (1..9)	*/
	printf("Iniciando proceso de adquisición.\n");
	SendcommSerie(fd, "AM1011Z");
	/** Esperamos la respuesta del Arduino al mensaje de inicio de adquisicion.*/
	/** En caso de no haber error, iniciamos la adquisicion de datos*/
	/** En caso de TIMEOUT en la comunicación, reintentamos 3 veces ante de salir*/
	/** Se puede habilitar variable y opcion de incio para el numero de reintentos*/
	while (1) {
		if (!recieveCommSerie(fd, buf)) {
			printf("Vovler a intentar comunicacion (y/n)?: ");
			c = getchar();
			if (c != 'y') {
				c = 'n';
				break;
			}
		}else
			adquisicion(fd, buf);
	}
	TancarSerie(fd);
	return 0;
}
