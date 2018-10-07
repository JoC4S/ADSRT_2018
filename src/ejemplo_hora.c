/***************************************************************************
                          exemple_segons_hora_local.c
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by A. Fontquerni
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
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>


/************************
 * *
 * * main
 * *
 * */

int main(void)
{
        time_t rawtime;
        struct tm *info;
        char buffer[80];

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
        printf("Formatted date & time : %s\n", buffer );

        return 0;
}
