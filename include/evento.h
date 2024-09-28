#ifndef EVENTO_H_
#define EVENTO_H_

#include <Arduino.h>

//Pines de los sensores
#define PIN_HUMEDAD             35
#define PIN_PRESION             34
#define PIN_PULSADOR            15
#define PIN_PULSADOR_APLAZAR    18
#define PIN_PULSADOR_CONFIRMAR  0

// Definición de Constantes
#define TIEMPO_INTERVALO_BUZZER 3000
#define TIEMPO_LEER_SENSORES    500     // Cada cuanto tiempo se leerán los sensores sin aplazo
#define TIEMPO_TIMEOUT          60000   // Cada cuanto tiempo se producirá un evento de timeout
#define UMBRAL_PRESION          250     // Valor de presión a detectar para que se considere que hay algo encima del sensor
#define UMBRAL_HUMEDAD          2000    // Valor de humedad a detectar para que se considere que haya orina en el papagayo

//Variables de tiempo de los sensores
extern unsigned long ultimo_tiempo_actual;
extern unsigned long tiempo_lectura_presion;
extern unsigned long tiempo_evento_timeout;

extern unsigned long temporizador_aplazo;

//Definición de Eventos
#define MAX_EVENTOS 7
enum eventos {EV_CONTINUE, EV_PULSO, EV_ORINO, EV_LEVANTO, EV_CONFIRMO, EV_APLAZO, EV_TIMEOUT};
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
bool sensar_timeout(bool forzar, unsigned long tiempo_actual);

#define MAX_LECTORES 6
#define MAX_LECTORES 6
typedef bool (*lectorSensor)(bool forzar, unsigned long tiempo_actual); // Definimos como deben ser las funciones para leer sensores
extern lectorSensor lector_sensor[MAX_LECTORES];

//Variable que indica si el paciente llamó o no a la enfermera
extern bool paciente_llamo;
extern bool aplazado;

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