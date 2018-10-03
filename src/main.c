/*!
   \file "main.c"
   \brief "Programa ADST_2018_1104934"
   \author "Jose Cayero"
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

int main(int argc, char **argv) {
								int oflag = 0; /** Variable a*/
								int bflag = 0; /** Variable b*/
								char *ovalue = NULL; /** Valor del parameto o*/
								char *cvalue = NULL; /** Valor del parameto c*/
								int index;
								int c;
								//	int opterr = 0;

								while ((c = getopt(argc, argv, "o:bc:h")) != -1)
																switch (c) {
																case 'o':    /** Opcion para seleccionar el nombre de la base de datos*/
																								oflag = 1;
																								ovalue = optarg;
																								break;
																case 'b':
																								bflag = 1;
																								break;
																case 'c':
																								cvalue = optarg;
																								break;
																case 'h':
																								printf("Uso: main [opciones] archivo...\n");
																								printf("-o	Abrir base de datos. Precisa 'nombre_base_datos.db'\n");
																								printf("-i	Insertar Tabla. Pracisa 'nombre_tabla'\n");
																								printf("-v	Inserta valor en tabla. Precisa.\n\n");
																								break;
																case '?':
																								if (optopt == 'c' || optopt == 'o')
																																printf("La opcion -%c requiere un argumento.\n", optopt);
																								else
																																printf("Opcion desconocida  '%c'.\n", optopt);
																								return 1;
																default:
																								abort();
																}

								printf("oflag = %d, ovalue = %s ,bflag = %d\n", oflag,ovalue, bflag );

								for (index = optind; index < argc; index++)
																printf("Non-option argument %s\n", argv[index]);
								sqlitefunc (1,ovalue);

								return 0;
}
/*!
   \brief Callback de la funcion sqlitefunc
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
   \param "Param description"
   \pre "Pre-conditions"
   \post "Post-conditions"
   \return "Return of the function"
 */
int sqlitefunc(int opcion, char* nombredb) {
								sqlite3 *db;
								char *zErrMsg = 0;
								int rc;
								char *sql;
								const char* data = "Callback function called";

								/* Open database */
								rc = sqlite3_open("test.db", &db);

								if( rc ) {
																fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
																return(0);
								} else {
																fprintf(stderr, "Base de datos abierta con exito.\n");
								}

								/* Create merged SQL statement */
								/** Ejecutaremos los SQL statmenst en funcion de la opcion que le pasemos a la funcion*/
								sql = "DELETE from COMPANY where ID=2; " \
														"SELECT * from COMPANY";

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
