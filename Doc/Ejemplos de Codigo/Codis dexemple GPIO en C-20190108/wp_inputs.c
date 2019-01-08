#include <wiringPi.h>
#include <stdio.h>

/*
 * Pinout
 +-----+-----+---------+------+---Pi 3---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+----++----+------+---------+-----+-----+
 |     |     |    3.3v |      |  1 || 2  |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 | ALT0 |  3 || 4  |      | 5v      |     |     |
 |   3 |   9 |   SCL.1 | ALT0 |  5 || 6  |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN |  7 || 8  | ALT5 | TxD     | 15  | 14  |
 |     |     |      0v |      |  9 || 10 | ALT5 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |  OUT | 11 || 12 | IN   | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 13 || 14 |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 15 || 16 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      | 17 || 18 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | ALT0 | 19 || 20 |      | 0v      |     |     |
 |   9 |  13 |    MISO | ALT0 | 21 || 22 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | ALT0 | 23 || 24 | OUT  | CE0     | 10  | 8   |
 |     |     |      0v |      | 25 || 26 | OUT  | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 | ALT0 | 27 || 28 | ALT0 | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 29 || 30 |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 31 || 32 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 33 || 34 |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 35 || 36 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 37 || 38 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      | 39 || 40 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+----++----+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | Physical | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---Pi 3---+------+---------+-----+-----+
 */
 
int pinL=22;    // pin #31 del connector #6 del BCM, out 3 del pimoroni Automation HAT
int pin=25;     // pin #37 del connector #26 del BCM, input 1 del pimoroni Automation HAT
int pin3=29;    // pin #40 del connector #21 del BCM, input 3 del pimoroni Automation HAT
int value;

int main (void)
{
  wiringPiSetup () ;
  pinMode (pinL, OUTPUT) ; 
  pinMode (pin, INPUT) ; 
  pinMode (pin3, INPUT) ; 
  pullUpDnControl (pin, PUD_OFF) ; 
  pullUpDnControl (pin3, PUD_OFF) ; 

	value=digitalRead(pin);
	printf("El valor del pin 1 és %d \n",value);
    delay (1000) ;

	value=digitalRead(pin3);
	printf("El valor del pin 3 és %d \n",value);
    delay (1000) ;
    
    digitalWrite (pinL,  HIGH) ; 
    delay (10) ;

	value=digitalRead(pin);
	printf("El valor del pin 1 és %d \n",value);
    delay (1000) ;

	value=digitalRead(pin3);
	printf("El valor del pin 3 és %d \n",value);
    delay (1000) ;
    
    digitalWrite (pinL,  LOW) ;
  
  return 0 ;
}
