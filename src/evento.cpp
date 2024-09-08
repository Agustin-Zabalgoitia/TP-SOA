#include <evento.h>

//No se pueden declarar valores de variables en el archivo header
//por lo que se tiene que ejecutar este código para que se
//declaren los valores de los eventos
unsigned long ultimo_tiempo_actual = 0;
unsigned long tiempo_lectura_presion = 0;
unsigned long tiempo_lectura_humedad = 0;
unsigned long tiempo_evento_llamada = 0;
unsigned long tiempo_evento_timeout = 0;
bool paciente_llamo = false;

//Guardamos los nombres de los eventos para informar por pantalla
String eventos_string[] = {"EV_CONT", "EV_ORINO", "EV_LEVANTO", "EV_PULSO", "EV_LLAMO", "EV_APLAZO", "EV_CONFIRMAR", "EV_TIMEOUT"};

//Guardamos los punteros de las funciones que leen el estado de los sensores
lectorSensor lector_sensor[] = {sensar_presion, sensar_humedad, consultar_llamada, sensar_pulsador, consultar_timeout};

//Creamos variables para guardar el último evento ocurrido y el nuevo evento
enum eventos nuevo_evento;
enum eventos ultimo_evento;

//Inicializamos el sensor DHT
DHT sensor_humedad(PIN_HUMEDAD, DHTYPE);

// Definir funciones para leer sensores
bool sensar_humedad(bool forzar, unsigned long tiempo_actual)
{
  if (tiempo_actual == 0)
    tiempo_actual = millis();

  // obtenemos el tiempo transcurrido entre el tiempo actual y la última vez que se midió la humedad
  unsigned long diferencia = (forzar) ? (TIEMPO_LEER_SENSORES) : (tiempo_actual - tiempo_lectura_humedad);

  if (diferencia >= TIEMPO_LEER_SENSORES)
  {
    tiempo_lectura_humedad = tiempo_actual;

    unsigned short valor_lectura = sensor_humedad.readHumidity(); //Lee la humedad

    if(valor_lectura >= UMBRAL_HUMEDAD)
    {
      nuevo_evento = EV_ORINO;
      return true;
    }
  }
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

//Lee el valor del pulsador
//No tiene ningún temporizador por lo que lo leerá cada vez
//que se ejecute el loop de get_event
bool sensar_pulsador(bool forzar, unsigned long tiempo_actual)
{
  bool valor_lectura = digitalRead(PIN_PULSADOR);
  if(valor_lectura)
    nuevo_evento = EV_PULSO;

  return valor_lectura;
}

//El pulsador del paciente funcionará de manera que quede un led encendido
//luego de presionar el botón. Para lograr esto se tendrá que definir un booleano
//que quede en estado de verdadero luego de presionar el botón. Esto no se puede hacer
//sin definir un evento porque rompería el patrón de diseño de máquina de estados.
//En este archivo .cpp solo se tendrían que detectar eventos, no acciones ni lógica.
//Eso corresponde al archivo estado.cpp
//Con esto en mente, voy a definir un nuevo evento llamado EV_LLAMO para esta función
//y voy a usar el evento EV_PULSO para detectar si el paciente pulsó el pulsador
bool consultar_llamada(bool forzar, unsigned long tiempo_actual)
{
  if(tiempo_actual == 0)
    tiempo_actual = millis();

  unsigned long diferencia = (forzar)?(TIEMPO_LEER_SENSORES):(tiempo_actual - tiempo_evento_llamada);

  if(diferencia >= TIEMPO_TIMEOUT)
  {
    tiempo_evento_llamada = tiempo_actual;

    if(paciente_llamo)
    {
      nuevo_evento = EV_LLAMO;
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

  if(diferencia >= TIEMPO_TIMEOUT)
  {
    tiempo_evento_timeout = tiempo_actual;
    nuevo_evento = EV_TIMEOUT; //Siempre ocurre un evento de timeout
    return true;
  }

  return false;
}