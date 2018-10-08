
String msgIn;                                    //Mensaje de entrada
String msgOut;                                   //Mensaje Salida
const size_t dataLength = sizeof(msgOut) / sizeof(msgOut[0]);

//Variables recibidas en los mensajes de entrada
int PinSD;                                     //Num de pin de sortida
int VPinS;                                     //Valor del pin de sortida (1 o 0)
int PinE;                                      //Num de pin d'entrada
int temps;                                     //tiempo adquisicion
int mostresMitjana;                            //numero de  muestras para realizar la media recibidas
int paroMarcha;                                // Dato recibido del mensaje tipo M  que indica si se ha de iniciar o parar la conversion

//Variables usadas para la ejecucion de los comandos solicitados
int ePinSalida;                                //Estado pin Salida
int muestreo;                                 // valor del tiempo de muestreo actual que se utiliza para la conversion
int conversion;                                // Conversion a 0, parada la adquisicón de datos, a 1 iniciada;
int contInterrupciones = 0;                     //contador de interrpciones para calcular tiempos superiores a los que permiten los 16bit el comparador
int numMostresMitjana = 1;                      //numero que define con cuantas muestras se hara el calculo
String  mitjana;                             //Mitjana calculada y converitda
int mitjaCalc;                                //Mitjana calculada
int valorMesurat;                             //Medida hecha en el pin analogico
int arrMuestras[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};              //Array de almacenamiento de las muestras
int i = 0;                                  //posicion de la ultima muestra en arrMuestras
String  codiRetorn;

//Variable para examen
int valorEx = 0;                           //valor recibido para analisis de sobrecalentamiento
int PinLed = 13;

void setup() {

  Serial.begin(9600); // abre el puerto serie,y le asigna la velocidad de 9600 bps

  //Se configuran los pins del 0 al 13 como salias.
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  // initialize timer1
  noInterrupts();  // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 62500; //62500 para 1s, 31250 para 0,5s valor de comparacion para el CTC (1/16exp6 * prescaler * OCR1A = Tiempo Interrupcion
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts

}

void loop()
{
  if (Serial.available() > 0)                                                 //Si hi ha alguna cosa escrita = 1
  {
    //msgIn = Serial.readString();
    filterMsg();                                                              //Se ejecuta la funcion para el filtrado de ruido en los caracteres de entrada.
    if (msgIn[0] == 'A')
    {
      switch (msgIn[1])
      {
        case 'S':                                                             //ASnnvZ Define el valor de pin nn a 1 o 0

          codiRetorn = checkMsgS(msgIn);
          msgOut = "AS" + codiRetorn + "Z";
          Serial.write (msgOut.c_str());          //la Funcion String.c_str(), pasa a puntero el string que pongamos.
          if (codiRetorn == "0")
          {
            ePinSalida = VPinS;
            digitalWrite(PinSD, VPinS);
            //********************************DEBUG salida*************************************************//
            //Serial.print( "Salida - "); Serial.print(PinSD); Serial.print (" -> "); Serial.println(VPinS);
            //*********************************************************************************************//
          }
          break;

        case 'E':                                                             //AEnnZ Comprueba el valor del pin solicitado

          codiRetorn = checkMsgE(msgIn);
          msgOut = "AE" + codiRetorn + digitalRead(PinE) + "Z";
          //********************************DEBUG salida*************************************************//
          Serial.print( "Salida - "); Serial.print(PinE); Serial.print (" = "); Serial.println(digitalRead(PinE));
          //*********************************************************************************************//
          break;

        case 'C':                                                             //ACZ solicita la media.

          codiRetorn = checkMsgC(msgIn);

          if (codiRetorn == "0")
          {
            mitjaCalc = calculoMedia();
            mitjana = String(mitjaCalc);                                      //convierte el valor int de mitjaCalc en un string en mitjana
            int mitjanaLen = mitjana.length();
            if (mitjanaLen < 4)                                               //Rellenamos con 0 las posiciones que falten en el integuer hasta tener 4 digitos
            {   
              for (i = 0; i < 4 - mitjanaLen; i++) mitjana = '0' + mitjana;
            }
            msgOut = "AC" + codiRetorn + mitjana + "Z";

            //********************************DEBUG salida*************************************************//
            //Serial.print( "Media= "); Serial.println(mitjana);
            //********************************DEBUG salida*************************************************//
          }
          else
            msgOut = "AC" + codiRetorn + "Z";
             Serial.write (msgOut.c_str());          //la Funcion String.c_str(), pasa a puntero el string que pongamos.
          break;

        case 'M':                   //AMcvvvZ Ininio Adquisicion

          codiRetorn = checkMsgM(msgIn);
          msgOut = "AM" + codiRetorn + "Z";

          if (codiRetorn == "0")
          {
            conversion = paroMarcha;
            muestreo = temps;
            numMostresMitjana = mostresMitjana;

            //********************************DEBUG salida*************************************************//
             //Serial.println( "********DEBUG***********");
            //Serial.print( "La conversion esta a : "); Serial.println(conversion);
            // Serial.print( "Tiempo de muestreo: "); Serial.println(muestreo);
            //Serial.print( "Muestras para calculo de la media: "); Serial.println( numMostresMitjana);
            //Serial.println( "********FIN DEBUG*******");
            //********************************DEBUG salida*************************************************//
          }
           Serial.write (msgOut.c_str());          //la Funcion String.c_str(), pasa a puntero el string que pongamos.
          break;
      }
        //Serial.print(msgOut);
      //Serial.write (msgOut.c_str());          //la Funcion String.c_str(), pasa a puntero el string que pongamos.
      //Serial.write ("Mensaje largo de prueba");
      msgIn = "";
      msgOut = "";
    }
  }
}

//**************************************************************************************************************
//RUTINA DE INTERRUPCION PARA TOMA DE MUESTRAS
ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  if (conversion == 1)
  {
    if (contInterrupciones < temps - 1)                      //Se incrementa el contador de interrupciones hasta llegar al tiempo indicado
    {
      contInterrupciones = contInterrupciones + 1;
      //********************************DEBUG salida*************************************************//
      //Serial.print(contInterrupciones);
      //********************************DEBUG salida*************************************************//
    }
    else                                                    //una vez se llega al tiempo de muestreo, se toma la muestra, y se mete en el array
    {
      valorMesurat = analogRead(A0);
      contInterrupciones = 0;
      if (i <= 7) i = i + 1;
      else i = 0;
      arrMuestras[i] = valorMesurat;
      //digitalWrite (13, digitalRead(13) ^ 1);
      //********************************DEBUG salida*************************************************//
      //Serial.print("<-"); Serial.print(arrMuestras[i]); Serial.print(" i ="); Serial.println(i);
      //********************************DEBUG salida*************************************************//
    }
  }
}

//FUNCION PARA FILTRAR MSG DE ENTRADA. La funcion encuentra el comando aun si se envian caracteres erroneos previos a este.
//Tiene la limitacion de que si se hay dos comandos en el buffer, solo tratara el primero y deshechara el resto. Punto a mejorar.

void filterMsg()
{
  String msgRaw;                                                  //Mensaje de entrada SIN TRATAR
  int msgLen;                                                     //num caracteres de msg
  int posA;
  int posZ;
  int n = 0;
  int out = 0;
  int Aencontrada = 0;

   
  msgRaw = Serial.readString();
  msgIn = "          ";                                           //Se inicializar MsgIn
  msgLen = msgRaw.length();

  while ((n < msgLen))
  {
    if (msgRaw[n] == 'A')
    {
      posA = n;
      Aencontrada = 1;
    }
    if ((msgRaw[n] == 'Z') && (Aencontrada == 1)) posZ = n;
    if ( posA < posZ) out = 1;
    n++;
    if (out == 1) break;
  }
  //Serial.print ("posA " ); Serial.println (posA);
  //Serial.print ("posZ " ); Serial.println (posZ);

  for (n = 0; n < posZ - posA + 1; n++)
  {
    msgIn[n] = msgRaw[posA + n];
 
  }
  //Serial.print ("Sortida:" );
  //Serial.println (msgIn);
}

//FUNION PARA CALCULO DE LA MEDIA
int calculoMedia()
{
  int n = 0;
  int pos = i;
  int calculTemp = 0;

  //Serial.println("*****debug calculo media*******");
  //Serial.print ("Muestras para hacer la media:");  Serial.println(numMostresMitjana);

  for (n = 0; n < numMostresMitjana; n++)
  {
    if (pos < 0) pos = 8;
    calculTemp = calculTemp + arrMuestras[pos];

    //Serial.print("Posicion array: "); Serial.print(pos); Serial.print(" Valor= "); Serial.println(arrMuestras[pos]);
    //Serial.print("Calculo: "); Serial.println(calculTemp);
    pos--;
  }
  calculTemp = calculTemp / numMostresMitjana;

  //Serial.println("*****debug calculo media*******");

  return calculTemp;
}

//Funcion que comprueba si el mensaje recibido tipo S es correcto y devuelve el codigo de retorno
char checkMsgS (String msgRecibido)
{
  char estado = '1';

  PinSD = (msgRecibido[2] - 48) * 10 + (msgRecibido[3] - 48);
  VPinS = msgRecibido[4] - 48;

  if (msgRecibido[5] == 'Z') estado = '0';
  else estado = '1';
  if ((PinSD > 13) || (PinSD < 0) || (VPinS > 1)) estado = '2';

  return estado;
}

//Funcion que comprueba si el mensaje recibido tipo E es correcto y devuelve el codigo de retorno
char checkMsgE (String msgRecibido)
{
  char estado = '1';

  PinE = (msgIn[2] - 48) * 10 + (msgIn[3] - 48);
  if ((PinE > 13) || (PinE < 0)) estado = '2';
  if (msgIn[4] == 'Z') estado = '0';
  else estado = '1';

  return estado;
}

//Funcion que comprueba si el mensaje recibido tipo C es correcto y devuelve el codigo de retorno
char checkMsgC (String msgRecibido)
{
  char estado = '1';

  if (msgRecibido[2] == 'Z') estado = '0';
  else estado = '1';

  return estado;
}

//Funcion que comprueba si el mensaje recibido tipo M es correcto y devuelve el codigo de retorno
char checkMsgM (String msgRecibido)
{
  char estado = '1';
  temps = ((msgRecibido[3] - 48) * 10) + (msgRecibido[4] - 48);
  mostresMitjana = msgRecibido[5] - 48;
  paroMarcha = msgRecibido[2] - 48;

  if (msgRecibido[6] == 'Z') estado = '0';
  else estado = '1';
  if ((paroMarcha > 1) || (paroMarcha < 0) || (temps > 20) || (temps < 1) || (mostresMitjana > 9) || (mostresMitjana < 1 )) estado = '2';

  return estado;
}
