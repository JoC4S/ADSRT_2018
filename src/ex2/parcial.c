/***************************************************************************
                          parcial.c
                             -------------------
    begin                : mar ene  9 16:00:04 CET 2002
    copyright            : (C) 2018 by Agusti Fontquerni
    email                : afontquerni@euss.es
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
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "libmail.h"

/*! \mainpage Fita 1
 *
 * \section intro_sec Introduction
 *
 * This is the introduction EUSS.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

/*
sqlite> .schema meteo

CREATE TABLE meteo(
  "Hora" TEXT,
  "TempMitjana" REAL,
  "TempMaxima" REAL,
  "Temp Minima" REAL,
  "Humitat" INTEGER,
  "Precipitacio" REAL,
  "VentVeloMitjana" REAL,
  "VentDireccioMitjana" INTEGER,
  "VentMaxim" REAL,
  "Pressio" REAL,
  "IrradianciaSolar" INTEGER
);

 */

#define MAXSTR 100

int g_verbose = 0;
char *g_bdname = "parcial.db";
char g_dbstr[MAXSTR];

struct meteo {
	int hminima;  //Humitat Minima
	float tmaxima;  //Temperatura Maxima
	float tminima;  //Temperatura mínima
	float tmitjana;  //Temperatura Mitjana
	float tvelomax;  //Velocitat maxima del vent
	int tdirventmax; //Direccio del vent en la velocitat màxima
	int tirrsolmax;	//Irradiancia Sola màxima
	char thora[5];	//Hora de la Irradiancia Solar màxima
} g_dades;

void sqlclean(void)
{
	memset( g_dbstr, 0, MAXSTR );

}

static int sqlcallback(void *NotUsed, int argc, char **argv, char
**azColName){
	int i;


	if(g_verbose) {
			printf("%s:%s\n", *azColName, argv[0]);
	}
	strcpy(g_dbstr, argv[0]);
	return 0;
}

int generar_informe(char *buffer) {

	strcpy(buffer, "Generant informe METEO\n" );
	strcat(buffer, "======================\n\n" );

	sprintf(buffer,
		"%s\
Humitat Mínima: %d\n\
Temperatura Maxima: %f\n\
Temperatura mínima: %f\n\
Temperatura Mitjana: %f\n\
Velocitat màxima del vent: %f\n\
Direcció del vent a velocitat màxima: %i\n\
Irradiancia Solar Màxima: %i \n\
Hora de la Irradiancia Solar màxima: %s \n\
\n",
		buffer,
		g_dades.hminima ,
		g_dades.tmaxima,
		g_dades.tminima,
		g_dades.tmitjana,
		g_dades.tvelomax,
		g_dades.tdirventmax,
		g_dades.tirrsolmax,
		g_dades.thora
		);

	strcat(buffer, "======================\n\n" );

	return 0;
}

int llegir_dades(void) {

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;

	rc = sqlite3_open(g_bdname, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n",
		sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}

	// -- Humitat Minima
	sqlclean();
	sql = "select MIN(Humitat) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.hminima = atoi(g_dbstr);

	// -- Temperatura Maxima
	sqlclean();
	sql = "select MAX(TempMaxima) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tmaxima = atof(g_dbstr);

	// -- Temperatura mínima
	sqlclean();
	sql = "select min(TempMinima) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tminima = atof(g_dbstr);

	// -- Temperatura Mitjana
	sqlclean();
	sql = "select MAX(TempMitjana) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tmitjana = atof(g_dbstr);

	// -- Velocitat màxima del vent
	sqlclean();
	sql = "select MAX(VentMaxim) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tvelomax = atof(g_dbstr);

	// -- Direcció del vent a velocitat màxima
	sqlclean();
	sql = "SELECT VentDireccioMitjana FROM meteo WHERE VentMaxim = (SELECT MAX(VentMaxim) from meteo);";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tdirventmax = atoi(g_dbstr);

	// -- Irradiancia Solar Màxima
	sqlclean();
	sql = "select MAX(IrradianciaSolar) from meteo;";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	g_dades.tirrsolmax = atoi(g_dbstr);

	// -- Hora de Irradiancia Solar màxima
	sqlclean();
	sql = "SELECT Hora FROM meteo WHERE IrradianciaSolar = (SELECT MAX(IrradianciaSolar) from meteo);";
	rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return 2;
	}
	strcpy(g_dades.thora, g_dbstr);




	sqlite3_close(db);
	return 0;
}


void print_usage(void) {
    printf("Usage: ./parcial --bdname FITXER [arguments]\n");
	printf("\n\t-b or --bdname [name] \tPath and database name\n");
    printf("\n   arguments:");
    printf("\n\t-h or --help");
    printf("\n\t-v or --verbose");
    printf("\n\n");
}

int input_parameters(int argc, char *argv[]) {
    int opt= 0;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
	{"bdname",	required_argument, 0,  'b' },
	{"help",	no_argument, 	0,  'h' },
	{"verbose",	no_argument, 	0,  'v' },
	{0,           0,                 0,  0   }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"b:hv",
		long_options, &long_index )) != -1) {
		switch (opt) {
			case 'b' :
				g_bdname = optarg;
				break;
			default:
			case 'h':
				print_usage();
				return 1;
			case 'v' :
				g_verbose = 1;
				break;
		}
    }

    return 0;
}


int main(int argc, char **argv)
{
	char buffer[10000];
	int result = 0;


	printf("Inici exercici parcial:\n\n");
	memset( &g_dades, 0, sizeof(struct meteo) );

	if(argc <2)	{
		print_usage();
		return 1;
	}

	if ( input_parameters(argc, argv) )
		return 0;

	if(g_verbose) printf("\n llegir_dades\n");
	llegir_dades();

	if(g_verbose) printf("\n generar_informe\n");
	generar_informe( buffer );
	printf( "%s", buffer );

	//Se envia por correo el informe a la direccion indicada
	//En el caso de mi libreria, el parametro EUSS, indica que el corre se envia no a traves de gmail
	//y usando stunnel, si no a nivel interno.
	sendmail("afontquerni@euss.es", buffer, EUSS);

	return 0;
}
