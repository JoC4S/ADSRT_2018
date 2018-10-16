#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/syscall.h>

void* print_time (void*);

timer_t timer1;
pthread_t thread;

int main (int argc, char **argv)
{
     pthread_create(&thread, NULL, print_time, NULL);

     pthread_join(thread, NULL);
     exit (EXIT_SUCCESS);
}

void* print_time (void* args)
{
     struct timespec tp;
     char buffer [80];
     struct itimerspec new_value, old_value;
     struct sigaction action;
     struct sigevent sevent;
     sigset_t set;
     int signum = SIGALRM;

     sevent.sigev_notify = SIGEV_THREAD_ID;
     sevent.sigev_notify_thread_id = syscall(__NR_gettid);
     sevent.sigev_signo = signum;

     sigemptyset(&set);
     sigaddset(&set, signum);
     sigprocmask(SIG_BLOCK, &set, NULL);

     if (timer_create (CLOCK_MONOTONIC, &sevent, &timer1) == -1)
             perror ("timer_create");

     new_value.it_interval.tv_sec = 1;
     new_value.it_interval.tv_nsec = 0;
     new_value.it_value.tv_sec = 1;
     new_value.it_value.tv_nsec = 0;

     if (timer_settime (timer1, 0, &new_value, &old_value) == -1)
         perror ("timer_settime");

     while(1){
         /* wait for signal (1 s) */
         if (sigwait (&set, &signum) == -1)
             perror ("sigwait");

         if (clock_gettime (CLOCK_MONOTONIC, &tp) == -1)
             perror ("clock_gettime");

         sprintf (buffer, "%ld s %ld ns overrun = %d\n", tp.tv_sec,
                 tp.tv_nsec, timer_getoverrun (timer1));
         write (STDOUT_FILENO, buffer, strlen (buffer));
     }
}
