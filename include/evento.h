#ifndef EVENTO_H_
#define EVENTO_H_

#include <Arduino.h>
#include <DHT.h> //Biblioteca necesaria para usar el sensor de humedad

//Pines de los sensores
#define PIN_HUMEDAD       15
#define PIN_PRESION       2
#define PIN_PULSADOR      4

//Inicializamos el sensor DHT
#define DHTYPE DHT22
//extern DHT sensor_humedad;

// Definición de Constantes
#define TIEMPO_LEER_SENSORES    1000    // Cada cuanto tiempo se leerán los sensores
#define UMBRAL_PRESION          1000    // Valor de presión a detectar para que se considere que hay algo encima del sensor
#define UMBRAL_HUMEDAD          50      // Valor de humedad a detectar para que se considere que haya orina en el papagayo

//Variables de tiempo de los sensores
extern unsigned long ultimo_tiempo_actual;
extern unsigned long tiempo_lectura_presion;
extern unsigned long tiempo_evento_timeout;

//Definición de Eventos
#define MAX_EVENTOS 7
enum eventos {EV_CONT, EV_ORINO, EV_LEVANTO, EV_PULSO, EV_APLAZO, EV_CONFIRMAR, EV_TIMEOUT};
extern String eventos_string[MAX_EVENTOS];

extern enum eventos nuevo_evento;
extern enum eventos ultimo_evento;

bool sensar_humedad(bool forzar, unsigned long tiempo_actual);
bool sensar_presion(bool forzar, unsigned long tiempo_actual);
bool consultar_timeout(bool forzar, unsigned long tiempo_actual);

#define MAX_LECTORES 3
typedef bool (*lectorSensor)(bool forzar, unsigned long tiempo_actual); // Definimos como deben ser las funciones para leer sensores
extern lectorSensor lector_sensor[MAX_LECTORES];

#endif