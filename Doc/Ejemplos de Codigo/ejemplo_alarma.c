#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>

void alarm_SerialCommtimeout(int signum){
  printf("Disparador alarma\n");

}

int main(){

  int i = 0;
  //set up alarm handler
  signal(SIGALRM, alarm_SerialCommtimeout);
  //schedule alarm for 1 second
  printf("previo alarma\n");
  for (i = 0; i < 3; i++) {
        alarm(3);
        printf("valor i: %d\n",i);
  }

  //do not proceed until signal is handled
  //pause();

}
