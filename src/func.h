void inicializaArray (float arrLecturas[3600]);
void llenarArray (float arrLecturas[3600], int ultimaMuestra, float temperatura);
float valorMaximo (float arrLecturas[3600], int ultimaMuestra);
float valorminimo (float arrLecturas[3600], int ultimaMuestra);
float promedioLecturas (float arrLecturas[3600], int ultimaMuestra, int numMuestasaPromediar);

#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[36m"
#define COLOR_RESET "\033[0m"


time_t t;
  struct tm *tm;
  char fechayhora[100];

//Inicializa el array de muestras a valor cero.
void inicializaArray (float arrLecturas[3600])
{
	int i;
	for (i=0;i<3600;i++)
		arrLecturas [i] = 0;
}
//Llenar el array de muestras con el valor de media solicitado a Aurduino
void llenarArray (float arrLecturas[3600], int ultimaMuestra, float temperatura)
{
	arrLecturas [ultimaMuestra] = temperatura;
	printf( COLOR_YELLOW "\n**********   DEBUG  *************");
	printf("\n   Muestra: %i --> Valor: %f ¤C",ultimaMuestra, arrLecturas[ultimaMuestra]);
	printf("\n*********  FIN DEBUG  ************\n" COLOR_RESET);
}
//Maximo
//Funcion a la que se le pasa el array de muestras, la ultima posicion guardada.
float valorMaximo (float arrValores[3600], int ultimaMuestra)
{
	int i;
	float Maximo = 0;
	for (i =0; i<ultimaMuestra;i++)
	{
		if (arrValores [i] > Maximo)
			Maximo = arrValores [i];
	}
	printf(COLOR_YELLOW "\n**********   DEBUG  *************");
	printf("\n   Maximo: %f ¤C\n", Maximo);
	printf("*********  FIN DEBUG  ************\n" COLOR_RESET);
	return Maximo;
}
//Minimo
//Funcion a la que se le pasa el array de muestras, la ultima posicion guardada, y devuelve el minimo
float valorminimo (float arrValores[3600], int ultimaMuestra)
{
	int i;
	float minimo = arrValores[0];
	for (i =0; i<ultimaMuestra;i++)
	{
		if (arrValores [i] < minimo)
		{
			minimo = arrValores [i];
		}
	}
	printf(COLOR_YELLOW "\n**********   DEBUG  *************");
	printf("\n   Minimo: %f ¤C\n", minimo);
	printf("*********  FIN DEBUG  ************\n" COLOR_RESET);
	return minimo;
}

//Media
//Funcion a la que se le pasa el array de muestras, la cantidad de muestras para promediar, el numero de muestras guardadas, y devuelve el promedio
float promedioLecturas (float arrValores[3600], int ultimaMuestra, int numMuestasaPromediar)
{
	int i;
	float suma = 0, media =0;
	int indice = ultimaMuestra-1;
	printf( COLOR_YELLOW "\n**********DEBUG: Funcion promedio*************");
	for (i =0; i<numMuestasaPromediar;i++)
	{
		if ((indice-i) < 0) indice = 3599+i;
		printf ("\n   Mustra: %i --> Valor: %02.2f ¤C",(indice-i), arrValores [indice -i]);
		suma += arrValores [indice - i];
		media = (suma / numMuestasaPromediar);
	}
	printf("\n   Media: %f ¤C. \n", media);
	printf("**********FIN DEBUG***************************\n" COLOR_RESET);
	return media;
}

//Imprime la hora por pantalla
#include <sys/time.h>
void printTime(void)
{

struct timeval timer_start;
	t= time(NULL);
	tm = localtime(&t);

	gettimeofday(&timer_start, NULL);
	char fechayhora[100];

	strftime(fechayhora, 100, "%H:%M:%S", tm);
	printf (COLOR_BLUE"\n[%s", fechayhora);

	printf (COLOR_BLUE":%03ld]", timer_start.tv_usec/1000);

	printf(COLOR_RESET);



}
