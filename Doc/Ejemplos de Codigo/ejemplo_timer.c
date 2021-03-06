/*
 * (c) EUSS 2013
 *
 *
 * Exemple d'utilització dels timers de la biblioteca librt
 * Crea dos timers que es disparen cada segon de forma alternada
 * Cada cop que es disparen imprimeixen per pantalla un missatge
 *
 * Per compilar: gcc exemple_timer.c -lrt -lpthread -o timer
 */
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (timer_callback) (union sigval);

/* Funció set_timer
 *
 * Crear un timer
 *
 * Paràmetres:
 * timer_id: punter a una estructura de tipus timer_t
 * delay: retard disparament timer (segons)
 * interval: periode disparament timer  (segons)
 * func: funció que s'executarà al disparar el timer
 * data: informació que es passarà a la funció func
 *
 * */
int set_timer(timer_t * timer_id, float delay, float interval, timer_callback * func, void * data)
{
    int status =0;
    struct itimerspec ts;
    struct sigevent se;

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = data;
    se.sigev_notify_function = func;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_REALTIME, &se, timer_id);

    ts.it_value.tv_sec = abs(delay);
    ts.it_value.tv_nsec = (delay-abs(delay)) * 1e09;
    ts.it_interval.tv_sec = abs(interval);
    ts.it_interval.tv_nsec = (interval-abs(interval)) * 1e09;

    status = timer_settime(*timer_id, 0, &ts, 0);
    return 0;
}

void callback(union sigval si)
{
    char * msg = (char *) si.sival_ptr;

    printf("%s\n",msg);
}

int main(int argc, char ** argv)
{

    timer_t tick;
    timer_t tock;
    set_timer(&tick, 1, 1, callback, (void *) "tick" );
    set_timer(&tock, 1.5, 1, callback, (void *) "tock" );
    getchar();

return 0;
}
