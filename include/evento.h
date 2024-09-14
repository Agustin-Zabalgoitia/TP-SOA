#ifndef EVENTO_H_
#define EVENTO_H_

#include <Arduino.h>
#include <DHT.h> //Biblioteca necesaria para usar el sensor de humedad

//Pines de los sensores
#define PIN_HUMEDAD             15
#define PIN_PRESION             2
#define PIN_PULSADOR            4
#define PIN_PULSADOR_APLAZAR    18
#define PIN_PULSADOR_CONFIRMAR  19

//Sensor de humedad
#define DHTYPE DHT22 //Tipo de sensor DHT
extern DHT sensor_humedad; //Objeto del sensor de humedad

// Definición de Constantes
#define TIEMPO_LEER_SENSORES    500     // Cada cuanto tiempo se leerán los sensores sin aplazo
#define TIEMPO_TIMEOUT          500     // Cada cuanto tiempo se producirá un evento de timeout
#define TIEMPO_APLAZO           120000  // Cada cuanto tiempo se leerán los sensores con aplazo
#define UMBRAL_PRESION          1000    // Valor de presión a detectar para que se considere que hay algo encima del sensor
#define UMBRAL_HUMEDAD          50      // Valor de humedad a detectar para que se considere que haya orina en el papagayo

//Variables de tiempo de los sensores
extern unsigned long ultimo_tiempo_actual;
extern unsigned long tiempo_lectura_presion;
extern unsigned long tiempo_evento_timeout;

extern unsigned long temporizador_aplazo;

//Definición de Eventos
#define MAX_EVENTOS 9
enum eventos {EV_CONT, EV_ORINO, EV_LEVANTO, EV_PULSO, EV_LLAMO, EV_APLAZO, EV_CONFIRMAR, EV_TIMEOUT, EV_TIMEOUT_APLAZO};
extern String eventos_string[MAX_EVENTOS];

extern enum eventos nuevo_evento;
extern enum eventos ultimo_evento;

//Funciones para detectar eventos
bool consultar_llamada(bool forzar, unsigned long tiempo_actual);
bool sensar_humedad(bool forzar, unsigned long tiempo_actual);
bool sensar_presion(bool forzar, unsigned long tiempo_actual);
bool sensar_llamada(bool forzar, unsigned long tiempo_actual);
bool sensar_aplazo(bool forzar, unsigned long tiempo_actual);
bool sensar_confirmacion(bool forzar, unsigned long tiempo_actual);
bool consultar_timeout(bool forzar, unsigned long tiempo_actual);
bool consultar_timeout_aplazo(bool forzar, unsigned long tiempo_actual);

#define MAX_LECTORES 8
typedef bool (*lectorSensor)(bool forzar, unsigned long tiempo_actual); // Definimos como deben ser las funciones para leer sensores
extern lectorSensor lector_sensor[MAX_LECTORES];

//Variable que indica si el paciente llamó o no a la enfermera
extern bool paciente_llamo;

//Variable para determinar si un evento se aplazó.
extern bool aplazado;

//Variable para conocer el tiempo en el que se inició un aplazo.
extern unsigned long tiempo_evento_timeout_aplazo;

//Estructura para guardar los datos de los pulsadores
struct pulsador
{
    unsigned short pin;
    bool estado_actual = LOW;
    bool estado_anterior = LOW;
};

//Variables para guardar los datos de los pulsadores
extern pulsador pulsadorLlamar, pulsadorAplazar, pulsadorConfirmar;

bool sensar_pulsador(pulsador*, eventos); //Función para obtener el valor de cualquier pulsador

#endif