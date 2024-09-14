#ifndef ESTADO_H_
#define ESTADO_H_

#include <evento.h>
#include <Wire.h>
#include "rgb_lcd.h"

//Pines de los actuadores
#define PIN_BUZZER        34
extern rgb_lcd lcd;

#define TIEMPO_ENTRE_CICLOS_DE_EJECUCION    50 //milisegundos

//Definición de los tonos del buzzer
#define TONO_SI         247
#define TONO_SOL        196
#define TONO_MI         165
#define DURACION_BUZZER 300 //en milisegundos

extern unsigned short ultimo_indice_lector_sensor;

#define MAX_ESTADOS 4
enum estados {ST_MONITOREO, ST_ORINADO, ST_LEVANTADO, ST_LLAMADO};
extern String estados_string[MAX_ESTADOS];

extern enum estados estado_actual;
extern enum estados ultimo_estado;

//Definición de enums para los colores del fondo del LCD
enum colores {AZUL, AMARILLO, ROJO};

//Funciones
void get_event                  ();
void fsm                        (); //Máquina de estados
void pausarActuadores           ();
void llamadaPaciente            ();
void confirmarLlamada           ();
void encenderFondoLCD    (colores);
void apagarFondoLCD             ();
//Funciones encargadas de informar, de momento mediante Serial.print, que ocurrió algo
void informarPulsoPaciente      ();
void informarConfirmacion       ();
void informarPausaActuadores    ();
void informarLevanto            ();
void informarOrino              ();

#endif