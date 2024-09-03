#include <evento.h>

//No se pueden declarar valores de variables en el archivo header
//por lo que se tiene que ejecutar esta función para que se
//y declaren los valores de los eventos
unsigned long ultimo_tiempo_actual = 0;
unsigned long tiempo_lectura_presion = 0;
unsigned long tiempo_evento_timeout = 0;

//Guardamos los nombres de los eventos para informar por pantalla
String eventos_string[] = {"EV_CONT", "EV_ORINO", "EV_LEVANTO", "EV_PULSO", "EV_APLAZO", "EV_CONFIRMAR", "EV_TIMEOUT"};

//Guardamos los punteros de las funciones que leen el estado de los sensores
lectorSensor lector_sensor[] = {sensar_humedad, sensar_presion, consultar_timeout};

enum eventos nuevo_evento;
enum eventos ultimo_evento;

// Definir funciones para leer sensores
bool sensar_humedad(bool forzar, unsigned long tiempo_actual)
{
  return false;
}

bool sensar_presion(bool forzar, unsigned long tiempo_actual)
{
  if (tiempo_actual == 0)
    tiempo_actual = millis();

  // obtenemos el tiempo transcurrido entre el tiempo actual y la última vez que se midió la presión
  unsigned long diferencia = (forzar) ? (TIEMPO_LEER_SENSORES) : (tiempo_actual - tiempo_lectura_presion);

  if (diferencia >= TIEMPO_LEER_SENSORES)
  {
    tiempo_lectura_presion = tiempo_actual;

    unsigned short valor_lectura = analogRead(PIN_PRESION);

    if (valor_lectura <= UMBRAL_PRESION)
    {
      nuevo_evento = EV_LEVANTO;
      return true;
    }
  }
  return false;
}

//Esta función se encarga de mandar cada un segundo un evento de timeout
bool consultar_timeout(bool forzar, unsigned long tiempo_actual)
{
  if(tiempo_actual == 0)
    tiempo_actual = millis();

  unsigned long diferencia = (forzar)?(TIEMPO_LEER_SENSORES):(tiempo_actual - tiempo_evento_timeout);

  if(diferencia >= TIEMPO_LEER_SENSORES)
  {
    tiempo_evento_timeout = tiempo_actual;
    nuevo_evento = EV_TIMEOUT; //Siempre ocurre un evento de timeout
    return true;
  }

  return false;
}