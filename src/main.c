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

/*!
   \brief "Funcion Principal"
   \param "Le pasamos patats, tomates y cebolla"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Nos devuelve un caldo"
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>

int dbfunc(char* opcion, char* nombredb);

int main(int argc, char *argv[]) {
								char *nombredb = NULL; /** Valor del parameto o*/
								char *ovlaue = NULL; /** Valor del parameto o*/
								int index;
								int c;
								int opterr = 0;

								while ((c = getopt(argc, argv, "n:o:h")) != -1)
																switch (c) {
																case 'n':
																								nombredb = optarg;
																								break;
																case 'o':
																								ovlaue = optarg;
																								break;
																case 'h':
																								printf("\nUso: main [opciones] archivo...\n");
																								printf("\n-n	Abrir base de datos. Precisa 'nombre_base_datos.db'\n");
																								printf("-o	Accion a realizar: 1 -Crear db. 2- Llenar Tabala.\n\n");
																								return 0;
																case '?':
																								if (optopt == 'n' || optopt == 'o')
																																printf("\nLa opcion -%c requiere un argumento.\n", optopt);
																								else
																																printf("Opcion desconocida  '%c'.\n", optopt);
																								printf("Usar '-h' para Ayuda.\n");
																								return 1;
																default:
																								abort();
																}

								//printf("oflag = %d, ovalue = %s ,bflag = %d\n", oflag,ovalue, bflag );
								for (index = optind; index < argc; index++)
																printf("Non-option argument %s\n", argv[index]);
								printf("nombre base de datos: %s\n",nombredb);
								if (optind == 1) {
																printf("Se necesitan parametros. Usar -h para Ayuda.\n");
																return 1;
								}

								dbfunc (ovlaue,nombredb);

								return 0;
}
/*!
   \brief "Callback de la funcion sqlitefunc"
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
 */
static int callback(void *data, int argc, char **argv, char **azColName) {
								int i;
								fprintf(stderr, "%s: ", (const char*)data);

								for(i = 0; i<argc; i++) {
																printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
								}
								printf("\n");
								return 0;
}
/*!
   \brief "Funcion para uso del sqlite"
   \param "Pasamos opcion y nombre de la base de datos."
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Devuelve integer"
 */
int dbfunc(char* opcion, char* nombredb) {
								sqlite3 *db;
								char *zErrMsg = 0;
								int rc;
								int c = atoi(opcion);
								char *sql = NULL;
								const char* data = "Callback function called";

								/* Open database */
								rc = sqlite3_open(nombredb, &db);

								if( rc ) {
																fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
																return(0);
								} else {
																fprintf(stderr, "Base de datos abierta con exito.\n");
								}

								/* Create merged SQL statement */
								/** Ejecutaremos los SQL statmenst en funcion de la opcion que le pasemos a la funcion*/
								switch (c) {

								case 1:     /** Crear Tabla en la base de datos*/
																sql =  "CREATE TABLE tabla1("  \
																						"HORA DATETIME       NOT NULL," \
																						"TEMPERATURA FLOAT     NOT NULL);";
																printf ("\n Crear tabla1 en %s.\n", nombredb);
																break;
								case 2:     /** Insertar datos en la tabla*/
																sql = "INSERT INTO tabla1 (HORA, TEMPERATURA) "  \
																						"VALUES ('2012-09­-10 18:56:10', 22.6 );";
																printf ("\n  Añadir entrada en %s-tabla1.\n\n", nombredb);
																break;
								case 3:     /** Consultar datos de la tabla*/
																sql = "SELECT * from tabla1";
								default:
																printf ("Ninguna acción a realizar con la base de datos.\n");
								}

								/* Execute SQL statement */
								rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

								if( rc != SQLITE_OK ) {
																fprintf(stderr, "SQL error: %s\n", zErrMsg);
																sqlite3_free(zErrMsg);
								} else {
																fprintf(stdout, "Operation done successfully\n");
								}
								sqlite3_close(db);
								return 0;
}
