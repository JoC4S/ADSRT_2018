/*!
   \file "informe.c"
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
#include <time.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sqlite3.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
//#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "libmail.h"

#define PRINT_TIME 1            /** Para funcion timestamp(). Idica que se imprimira la hora por patnalla.*/
#define horaInicio  0           /** Opción para funcion Query. Solicita hora de inicio.*/
#define horaFinal  1            /** Opción para funcion Query. Solicita hora de final.*/
#define tempFanOn 3             /** Opción para funcion Query. Solicita tiempo que ha estado funcionando el ventilador.*/
#define maxTemp  4              /** Opción para funcion Query. Solicita la máxima temperatura registrada.*/
#define minTemp 5               /** Opción para funcion Query. Solicita la mínima temperatura registrada.*/
#define avgTemp 6               /** Opción para funcion Query. Solicita la temperatura media de todo el registro.*/
#define horaInicioAlarm  7      /** Opción para funcion Query. Solicita hora de inicio de las alarmas.*/
#define horaFinalAlarm  8       /** Opción para funcion Query. Solicita hora final de las alarmas.*/
#define tempFanOnAlarm 9        /** Opción para funcion Query. Solicita tiempo que ha estado funcionando el ventilador con estado de alarma.*/

#define d_ayer 2                /** parametro para funcion timestamp(). Hace que la funcion devuelva la fecha del sistema del dia anterior*/
#define d_hoy 4                 /** parametro para funcion timestamp(). Hace que la funcion devuelva la fecha del sistema del dia en curso*/



#define COLOR_GREEN "\x1B[32m"
#define COLOR_RED "\x1B[31m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_RESET "\033[0m"

char informe[500];
char mail_informe[2000];

int tvalue = 10;                                /** Valor del tiempo de muestreo de temperatura. Ha de ser igual.*/
sqlite3 *db;                                    /** variable sqlite3 para la base de datos.*/
char *nombredb = NULL;                          /** Valor del parameto n*/

/*!
   \brief "Obtencion de fecha con formato sqlite DATETIME"
   \param "print: devuelve dia y hora actual, ayer: devuelve la fecha - 24h", antesdeayer - 48h
   \return "Devuelve puntero a un string con la fecha"
 */
int get_str_timestamp(int print, char *buffer)
{
	time_t rawtime, diaprevio;
	struct tm *info, *dia_anterior;
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

	switch (print) {
	case d_ayer:
		/** Resta 24h a la fecha actual*/
		diaprevio = rawtime - 86400;
		dia_anterior = localtime(&diaprevio);
		strftime(buffer, 80, "%Y-%m-%d", dia_anterior);
		break;
	case d_hoy:
		/** Obtiene la fecha actual*/
		strftime(buffer, 80, "%Y-%m-%d", info);
		break;
	case 1:
		strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);
		printf(COLOR_YELLOW "%s" COLOR_RESET, buffer );
		break;
	default:
		strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);
		break;
	}
	retp = buffer;
	return retp;
}
/*!
   \brief "Abre la base de datos"
   \param "char nombredb[]"
   \pre   "La base de datos debe existir"
   \return "FAlse = OK, True = ERROR"
 */
int abrirdb()
{
	/** Abrimos la base de datos.*/
	int rc;

	printf("Abriendo base de datos '%s'...\n", nombredb);
	rc = sqlite3_open(nombredb, &db);
	if ( rc ) {
		fprintf(stderr, COLOR_RED "---> No se puede abrir la base de datos: %s\n" COLOR_RESET, sqlite3_errmsg(db));
		return 0;
	} else {
		if (nombredb == NULL) {
			fprintf(stderr, COLOR_RED " ERROR: Se necesita un nombre para la base de datos.\n" COLOR_RESET);
			return 1;
		}else {
			printf("\n<==========================================================>\n");
			get_str_timestamp(PRINT_TIME,NULL);
			printf("---> Base de datos abierta.\n");
		}
	}
	return 0;
}

int callback_horaInicio(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Hora Inicio = %s.\n", argv[0] );
	sprintf(informe, "Hora Inicio = %s.\n", argv[0]);
	strcat(mail_informe, informe);
	return 0;
}

int callback_horaInicioAlarmas(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Hora Inicio Alarmas = %s.\n", argv[0] );
	sprintf(informe, "Hora Inicio Alarmas = %s.\n", argv[0] );
	strcat(mail_informe, informe);
	return 0;
}

int callback_horaFinalAlarmas(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Hora Final Alarmas = %s.\n", argv[0] );
	sprintf(informe, "Hora Final Alarmas = %s.\n", argv[0] );
	strcat(mail_informe, informe);
	return 0;
}

int callback_horaFinal(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Hora Final = %s.\n", argv[0] );
	sprintf(informe, "Hora Final = %s.\n", argv[0] );
	strcat(mail_informe, informe);
	return 0;
}

int callback_fanOn(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	int fanOn;
	int fanOnAvg;

	if (argv[0] != NULL){
		fanOn = atoi(argv[0]) * 10;
		fanOnAvg = (atoi(argv[0]) * 10 / atoi(argv[0]));
	}else {
		fanOn = 0;
		fanOnAvg = 0;
	}
	printf("Temp Fan On = %d s.\n", fanOn );
	sprintf(informe, "Temp Fan On = %d s.\n", fanOn);
	strcat(mail_informe, informe);
	printf("Temp AVG On = %d s.\n", fanOnAvg);
	sprintf(informe, "Temp AVG On = %d s.\n", fanOnAvg);
	strcat(mail_informe, informe);
	return 0;
}

int callback_ventAlarmOn(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Temp Fan Alarm On = %s min.\n", argv[0] );
	sprintf(informe, "Temp Fan Alarm On = %s min.\n", argv[0]);
	strcat(mail_informe, informe);
	return 0;
}

int callback_maxtemp(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Máximo = %s ºC\n", argv[0] );
	sprintf(informe, "Máximo = %s ºC\n", argv[0] );
	strcat(mail_informe, informe);
	return 0;
}

int callback_mintemp(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("mínimo = %s ºC\n", argv[0] );
	sprintf(informe, "mínimo = %s ºC\n", argv[0]  );
	strcat(mail_informe, informe);
	return 0;
}

int callback_avgtemp(void *NotUsed, int argc, char **argv, char **azColName)
{
	NotUsed = 0;
	printf("Media = %s ºC\n", argv[0] );
	sprintf(informe, "Media = %s ºC\n", argv[0] );
	strcat(mail_informe, informe);
	return 0;
}
/*!
   \brief Realiza query a la base de datos.
   \param Opcione: horaInicio, horaFinal, tampFanOn, maxTemp, minTemp, avgTemp
   \return "Return of the function"
 */
int query(int opcion)
{
	int rc;
	char ayer[80];                                                          /** Almacena la fecha y hora de 24 h atras*/
	char hoy[80];
	char sql[100];
	char *err_msg = 0;

/** asigna adaybefore la misma hora actual del sistema pero 48 horas antes*/
	memset( hoy, 0, 80 );
	get_str_timestamp(d_hoy, hoy);
	memset( hoy, 0, 80 );
	get_str_timestamp(d_ayer, ayer);
	printf ("variable hoy: %s\n",hoy);
	printf ("variable ayer: %s\n",ayer);

	switch (opcion) {
	case horaInicio:
		sprintf (sql, "SELECT MIN(HORA) FROM DATOS WHERE HORA > '%s'AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_horaInicio, 0, &err_msg);
		break;
	case horaFinal:
		sprintf(sql,"SELECT MAX(HORA) FROM DATOS WHERE HORA > '%s' AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_horaFinal, 0, &err_msg);
		break;
	case tempFanOn:
		sprintf(sql,"SELECT SUM(VENTILADOR) FROM DATOS WHERE HORA > '%s' AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_fanOn, 0, &err_msg);
		break;
	case maxTemp:
		sprintf(sql, "SELECT MAX(TEMPERATURA) FROM DATOS WHERE HORA > '%s' AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_maxtemp, 0, &err_msg);
		break;
	case minTemp:
		sprintf(sql, "SELECT MIN(TEMPERATURA) FROM DATOS WHERE HORA > '%s' AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_mintemp, 0, &err_msg);
		break;
	case avgTemp:
		sprintf(sql,"SELECT AVG(TEMPERATURA) FROM DATOS WHERE HORA > '%s' AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_avgtemp, 0, &err_msg);
		break;
	case horaInicioAlarm:
		sprintf(sql, "SELECT MIN(HORA) FROM ALARMAS WHERE HORA > '%s';",ayer);
		rc = sqlite3_exec(db, sql, callback_horaInicioAlarmas, 0, &err_msg);
		break;
	case horaFinalAlarm:
		sprintf(sql, "SELECT MAX(HORA) FROM ALARMAS WHERE HORA  > '%s' AND HORA < '%s';",ayer,hoy);
		rc = sqlite3_exec(db, sql, callback_horaFinalAlarmas, 0, &err_msg);
		break;
	case tempFanOnAlarm:
		sprintf(sql, "SELECT MAX(TEMP_FAN_ON) FROM ALARMAS WHERE HORA > '%s' AND HORA < '%s';",ayer,hoy);
		//printf("comando SQL minTemp: %s\n",sql);
		rc = sqlite3_exec(db, sql, callback_ventAlarmOn, 0, &err_msg);
		break;
	}

	if (rc != SQLITE_OK ) {
		fprintf(stderr, "Failed to select data\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;

}

int main(int argc, char *argv[])
{
	int dvalue = 0;
	int c;

	while ((c = getopt(argc, argv, "n:t:hd")) != -1) {
		switch (c) {
		case 'n':
			nombredb = optarg;
			break;
		case 'd':                                                                                                                                                         /** Activa el modo debug*/
			dvalue = 1;
			break;
		case 't':                                                                                                                                                         /** Define el tiempo de muestreo*/
			tvalue = atoi(optarg);
			break;
		case 'h':
			printf("\nUso: main [opciones] -n [nombre_archivo.db]...\n\n");
			printf("-n [nombre.db]	Nombre base de datos. Precisa 'nombre_base_datos.db'\n");
			printf("-t [seg]	Tiempo de muestreo en sgundos. Default = 10s.\n");
			printf("-d              Modo Debug.\n\n");
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
	/** Se abre la base de datos.*/
	if (abrirdb()) {
		printf("\n No se ha encontrado la base de datos.\n");
		exit(1);
	}
	printf("\n------------- TABLA DATOS -------------------\n\n");
	query(horaInicio);
	query(horaFinal);
	query(maxTemp);
	query(minTemp);
	query(avgTemp);
	query(tempFanOn);
	printf("\n------------- TABLA ALARMAS -----------------\n");
	query(horaInicioAlarm);
	query(horaFinalAlarm);
	query(tempFanOnAlarm);
	printf("\n<==========================================================>\n");
	get_str_timestamp(PRINT_TIME, NULL);
	/** Se ciera la base de datos.*/
	sqlite3_close(db);
	printf("---> Base de datos cerrada.\n");
	printf("---> Mandando Informe.\n");
	char mailto[] = "1104934@campus.euss.org";
	printf("%s",mail_informe);
	sendmail(mailto, mail_informe, EUSS);

	return 0;
}
