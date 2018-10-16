
    #include <stdio.h>
    #include <time.h>
    #include <signal.h>
timer_t gTimerid1;
struct sigevent sigv;
siginfo_t SIGTEM1;

void start_timer1(void)
{
	struct itimerspec value;
	value.it_value.tv_sec = 5;//waits for 5 seconds before sending timer signal
	value.it_value.tv_nsec = 0;
	value.it_interval.tv_sec = 5;//sends timer signal every 5 seconds
	value.it_interval.tv_nsec = 0;

        sigv.sigev_notify = SIGEV_SIGNAL;
        sigv.sigev_signo = SEG;
	if (!timer_create (CLOCK_REALTIME, &sigv, &gTimerid1)) {
		printf("Creado timer 'start_time1'\n" );
	}else
		printf("Error al setear 'timer_create'\n" );
	timer_settime (gTimerid1, 0, &value, NULL);
}
void stop_timer(void)
{
	struct itimerspec value;
	value.it_value.tv_sec = 5;
	value.it_value.tv_nsec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_nsec = 0;
	timer_settime (gTimerid1, 0, &value, NULL);
}
void timer_callback1(int sig) {
	printf(" Catched timer signal 1: %d ... !!\n", sig);
}

int main(int ac, char **av)
{
	char c = '\0';
	signal(SIGTEM1.si_signo, timer_callback1);
	//signal(SIGALRM, timer_callback2);
	printf("Inicio Main:\n");
	start_timer1();
	//start_timer2();
	while(1){}
}
