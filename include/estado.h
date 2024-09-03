#ifndef ESTADO_H_
#define ESTADO_H_

#include <evento.h>

//Pines de los actuadores
#define PIN_BUZZER        0
#define PIN_LED_AMARILLO  17
#define PIN_LED_AZUL      16
#define PIN_LED_ROJO      5

#define TIEMPO_ENTRE_CICLOS_DE_EJECUCION    50 //milisegundos

extern unsigned short ultimo_indice_lector_sensor;

#define MAX_ESTADOS 4
enum estados {ST_MONITOREO, ST_ORINADO, ST_LEVANTADO, ST_PULSADO};
extern String estados_string[MAX_ESTADOS];

extern enum estados estado_actual;
extern enum estados ultimo_estado;

void get_event();
void fsm();

#endif