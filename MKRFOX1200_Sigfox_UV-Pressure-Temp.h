// Descomentar para hacer debug
#define DEBUG_SIGFOX 1
 
// Librerías
#include <SFE_BMP180.h>
#include <Wire.h>
#include <ArduinoLowPower.h>
#include <SigFox.h>
 
// Sensor de temperatura y presión
const int numBmp180 = 50; // Número de muestras
SFE_BMP180 bmp180;
float temps[numBmp180]; // Array para almacenar lecturas
int tempActual = 0; // Lectura por la que vamos
float tempTotal = 0; // Total de las que llevamos
float tempMedia = 0; // Media de las medidas
bool tempPrimera = false; // Para saber que ya hemos calculado por lo menos una
 
float presis[numBmp180]; // Array para almacenar lecturas
float presisTotal = 0; // Total de las que llevamos
float presisMedia = 0; // Media de las medidas
 
// Sensor de UV
int UVOUT = A1; // Entrada del sensor
int REF_3V3 = A2; // Entrada de referencia 3,3V
 
const int numUv = 50; // Número de muestras
float uvs[numUv]; // Array para almacenar lecturas
int uvActual = 0; // Lectura por la que vamos
float uvTotal = 0; // Total de las que llevamos
float uvMedia = 0; // Media de las medidas
bool uvPrimera = false; // Para saber que ya hemos calculado por lo menos una
 
// Estructura para almacenar datos
typedef struct __attribute__ ((packed)) sigfox_message {
  uint8_t status;
  int16_t bmpTemperature;
  uint16_t bmpPressure;
  float sparkUv;
  uint8_t lastMessageStatus;
} SigfoxMessage;
 
SigfoxMessage msg;
 
// Centinela para saber cuando transmitir 10 minutos mínimo tomando muestras
bool empiezaTransmitir = false;
unsigned long tiempoInicial = 0;
unsigned long tiempoEstimado = 5 * 60 * 1000; //5 minutos
 
void setup() {
  Serial.println("void setup");
#ifdef DEBUG_SIGFOX
  Serial.begin(9600);
#endif
  /********* Sigfox ************/
  if (!SigFox.begin()) {
    // Something is really wrong, try rebooting
    // Reboot is useful if we are powering the board using an unreliable power source
    // (eg. solar panels or other energy harvesting methods)
    reboot();
  }
 
  // Ponemos el módulo en estado reposo hasta que mandemos el mensaje
  SigFox.end();

  Serial.println("iniciando BMP180");
  /************ SETUP BMP180 **************/
  // Iniciamos el sensor de temperatura y presión
  if (bmp180.begin())
  {
#ifdef DEBUG_SIGFOX
    Serial.println("BMP180 iniciado");
#endif
  }
  else
  {
    Serial.println("BMP180 ha fallado\n\n");
#ifdef DEBUG_SIGFOX
    // Algo ha ido mal
    Serial.println("BMP180 ha fallado\n\n");
#endif
    while (1); // Bucle infinito.
  }
 
  // Inicializamos el array media temperatura y presión
  for (int i = 0; i < numBmp180; i++)
  {
    temps[i] = 0;
    presis[i] = 0;
  }
 
  // Inicializamos el array media radiación UV
  for (int i = 0; i < numUv; i++)
  {
    uvs[i] = 0;
  }
 
  /********* SETUP UV ************/
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
 
  // Debugamos Sigfox
  SigFox.debug();
 
  // Obtenemos el tiempo inicial
  tiempoInicial = millis();
}
 
void loop() {
  // Medir temperatura
  medirTemp();
  // Medir radiación UV
  medirUV();
 
  // Si ha pasado el tiempo establecido marcamos para empezar a transmitir
  if (millis() - tiempoInicial >= tiempoEstimado)
  {
    empiezaTransmitir = true;
  }
 
  // Comprobamos que tengamos todas las medidas y que haya pasado el tiempo estimado
  if (uvPrimera && tempPrimera && empiezaTransmitir)
  {
#ifdef DEBUG_SIGFOX
    Serial.print("Temperatura: ");
    Serial.print(tempMedia, 2);
    Serial.print(" *C , ");
    Serial.print("Presion: ");
    Serial.print(presisMedia, 2);
    Serial.println(" mb");
    Serial.print("Radiación UV (mW/cm^2): ");
    Serial.println(uvMedia);
    Serial.print("Milisegundos: ");
    Serial.println(millis());
    Serial.print("Module temperature: ");
    Serial.println(SigFox.internalTemperature());
#endif
 
    // Almacenamos la información para enviar
    msg.bmpTemperature = tempMedia;
    msg.bmpPressure = presisMedia;
    msg.sparkUv = uvMedia;
 
    // Reseteamos los contadores
    tempPrimera = false;
    uvPrimera = false;
    empiezaTransmitir = false;
    tempActual = 0;
    uvActual = 0;
 
    // Start the module
    SigFox.begin();
    // Wait at least 30ms after first configuration (100ms before)
    delay(100);
 
    // Clears all pending interrupts
    SigFox.status();
    delay(1);
 
    SigFox.beginPacket();
    SigFox.write((uint8_t*)&msg, 12);
 
    msg.lastMessageStatus = SigFox.endPacket(true);
#ifdef DEBUG_SIGFOX
    Serial.println("Status: " + String(msg.lastMessageStatus));
#endif
 
    SigFox.end();
 
    //A dormir 10 minuto
    LowPower.sleep(10 * 60 * 1000);
 
    // Empezamos a contar el tiempo
    tiempoInicial = millis();
  }
  else
  {
    delay(200);
  }
}
 
/*************Sigfox**************/
void reboot() {
  Serial.println("void reboot");
  NVIC_SystemReset();
  while (1);
}
 
void medirUV()
{
  // Eliminamos la última medida
  uvTotal = uvTotal - uvs[uvActual];
 
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
 
  // Calculo de la radiacion con la referencia de 3,3V
  float outputVoltage = 3.3 / refLevel * uvLevel;
 
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); // Conversión entre voltaje y radiación UV
 
  // Almacenamos la distancia en el array
  uvs[uvActual] = uvIntensity;
 
  // Añadimos la lectura al total
  uvTotal = uvTotal + uvs[uvActual];
 
  // Avanzamos a la siguiente posición del array
  uvActual = uvActual + 1;
 
  // Comprobamos si hemos llegado al final del array
  if (uvActual >= numUv)
  {
    uvPrimera = true;
    uvActual = 0;
  }
 
  // Calculamos la media
  uvMedia = uvTotal / numUv;
}
 
// Calcula la media de cada pin
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 10;
  unsigned int runningValue = 0;
 
  for (int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;
 
  return (runningValue);
}
 
// Función map con floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
 
void medirTemp()
{
  char status;
  double T, P;
 
  // Eliminamos la última medida
  tempTotal = tempTotal - temps[tempActual];
  presisTotal = presisTotal - presis[tempActual];
 
  status = bmp180.startTemperature(); //Inicio de lectura de temperatura
  if (status != 0)
  {
    delay(status); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); //Obtener la temperatura
    if (status != 0)
    {
      status = bmp180.startPressure(3); //Inicio lectura de presión
      if (status != 0)
      {
        delay(status); //Pausa para que finalice la lectura
        status = bmp180.getPressure(P, T); //Obtenemos la presión
        if (status != 0)
        {
          // Almacenamos la temperatura en el array
          temps[tempActual] = (float)T;
          // Almacenamos la presión en el array
          presis[tempActual] = (float)P;
 
          // Añadimos la lectura al total
          tempTotal = tempTotal + temps[tempActual];
 
          // Añadimos la lectura al total
          presisTotal = presisTotal + presis[tempActual];
 
          // Avanzamos a la siguiente posición del array
          tempActual = tempActual + 1;
 
          // Comprobamos si hemos llegado al final del array
          if (tempActual >= numBmp180)
          {
            tempPrimera = true;
            tempActual = 0;
          }
 
          // Calculamos la media
          tempMedia = tempTotal / numBmp180;
          presisMedia = presisTotal / numBmp180;
        }
      }
    }
  }
}
