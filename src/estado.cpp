#include <estado.h>
#include <evento.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

unsigned short ultimo_indice_lector_sensor = 0;
String estados_string[] = {"ST_MONITOREO", "ST_ORINADO", "ST_LEVANTADO", "ST_LLAMADO"};
enum estados estado_actual = ST_MONITOREO; // Inicializa el estado actual
enum estados ultimo_estado;

void get_event() {
    // Leemos los sensores
    unsigned short indice = 0;
    unsigned long tiempo_actual = millis();
    unsigned long diferencia = (tiempo_actual - ultimo_tiempo_actual);
    bool timeout = (diferencia > TIEMPO_ENTRE_CICLOS_DE_EJECUCION) ? true : false;

    if (timeout) {
        timeout = false;
        ultimo_tiempo_actual = tiempo_actual;

        // Verificar si el aplazo ha llegado a su timeout antes de chequear los sensores
        if (consultar_timeout_aplazo(false, tiempo_actual)) {
            // Si se detectó un timeout de aplazo, la función ya habrá configurado el evento correspondiente
            return; // Salir para manejar este evento en la FSM
        }

        indice = (ultimo_indice_lector_sensor % MAX_LECTORES);
        ultimo_indice_lector_sensor++;

        if (lector_sensor[indice](false, tiempo_actual)) {
            return; // Si los sensores detectaron un evento, salimos para atenderlo en FSM
        }
    }

    // Si los sensores no detectaron un nuevo evento y no hay timeout de aplazo, continuamos
    nuevo_evento = EV_CONT;
}

void fsm() {
    
    get_event();

    switch (estado_actual) {
        case ST_MONITOREO: {
            if (nuevo_evento == EV_CONT && ultimo_evento != EV_CONT) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Monitoreando...");
            }

            switch (nuevo_evento) {
                case EV_ORINO:
                    informarOrino();      
                    estado_actual = ST_ORINADO;
                    break;

                case EV_LEVANTO:
                    informarLevanto();    
                    estado_actual = ST_LEVANTADO;
                    break;

                case EV_PULSO:
                    llamadaPaciente();    
                    estado_actual = ST_LLAMADO;
                    break;

                case EV_TIMEOUT_APLAZO_ORINADO:
                    informarOrino();      
                    estado_actual = ST_ORINADO;
                    break;

                case EV_TIMEOUT_APLAZO_PULSADO:
                    llamadaPaciente();    
                    estado_actual = ST_LLAMADO;
                    break;

                case EV_LLAMO:
                    
                    if (paciente_llamo) {
                        llamadaPaciente();   
                        estado_actual = ST_LLAMADO;
                        paciente_llamo = false;  
                    }
                    break;

                case EV_BOTON_CONFIRMAR:
                    confirmarLlamada();   
                    estado_actual = ST_MONITOREO; 
                    break;

                default:
                    break;
            }
            break;
        }
        
        case ST_LEVANTADO: {
            switch (nuevo_evento) {
                case EV_CONT:
                    digitalWrite(PIN_LED_ROJO, HIGH); 
                    break;

                case EV_TIMEOUT:
                    
                    apagarTodosLosLEDs();  
                    estado_actual = ST_MONITOREO;  
                    break;

                case EV_CONFIRMAR:
                    confirmarLlamada();
                    apagarTodosLosLEDs();
                    estado_actual = ST_MONITOREO;  
                    break;

                default:
                    break;
            }
            break;
        }

        case ST_ORINADO: {
            digitalWrite(PIN_LED_AMARILLO, HIGH); 
            switch (nuevo_evento) {
                case EV_CONT:
                   
                    break;

                case EV_APLAZO:
                    aplazar(ST_ORINADO);
                    apagarTodosLosLEDs();  
                    estado_actual = ST_MONITOREO;
                    break;

                case EV_CONFIRMAR:
                    confirmarLlamada();
                    apagarTodosLosLEDs();  
                    estado_actual = ST_MONITOREO;
                    break;

                default:
                    break;
            }
            break;
        }

        case ST_LLAMADO: {
            digitalWrite(PIN_LED_AZUL, HIGH);  
            switch (nuevo_evento) {
                case EV_CONT:
                    reproducirMelodia();
                    break;

                case EV_APLAZO:
                    aplazar(ST_LLAMADO);  
                    apagarTodosLosLEDs(); 
                    estado_actual = ST_MONITOREO;
                    break;

                case EV_CONFIRMAR:
                    confirmarLlamada();  
                    apagarTodosLosLEDs();
                    estado_actual = ST_MONITOREO;
                    break;

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    ultimo_evento = nuevo_evento;  
}

// Definición de funciones propias
void aplazar(estados estado) {
    pausarActuadores();
    ultimo_estado = estado;
    aplazado = true;
    tiempo_evento_timeout_aplazo = millis();
    
    Serial.print("Aplazado en: ");
    Serial.println(tiempo_evento_timeout_aplazo); 

    nuevo_evento = EV_CONT; 
}

void volverDelAplazo() {
    reanudarActuadores();
    aplazado = false;
    tiempo_evento_timeout_aplazo = 0;

    // Vuelve al ultimo estado
    estado_actual = ultimo_estado; 
}

void pausarActuadores() {
    tone(PIN_BUZZER, TONO_SOL, DURACION_BUZZER);
    informarPausaActuadores();
}

void reanudarActuadores() {
    informarReanudacionActuadores();
}

// Define las notas que se usaran en la melodia
int melodia[] = {
    262,
    330, 
    392,  
    262   
};

// Define las duraciones de las notas (esto es para generar la melodía)
int duraciones[] = {
    300,
    300,  
    300,  
    600   
};

int numeroNotas = sizeof(melodia) / sizeof(melodia[0]);
int notaActual = 0;             // Índice de la nota actual en la melodía
unsigned long inicioNota = 0;   // Marca de tiempo de inicio de la nota actual
bool reproduciendoMelodia = false;

// logica para manejar la melodía.
// la idea es que use millis para no bloquear la ejecución y que el SE siga monitoreando y respondiendo a eventos, mientras la melodía se reproduce.
void reproducirMelodia() {
    unsigned long tiempoActual = millis();

    // Verifica si la melodía debe comenzar
    if (!reproduciendoMelodia) {
        notaActual = 0;
        inicioNota = tiempoActual;
        tone(PIN_BUZZER, melodia[notaActual]);
        reproduciendoMelodia = true;
    }

    // 
    if (reproduciendoMelodia && (tiempoActual - inicioNota >= duraciones[notaActual])) {
        notaActual++;  
        if (notaActual < numeroNotas) {
            inicioNota = tiempoActual;  
            tone(PIN_BUZZER, melodia[notaActual]);
        } else {
            noTone(PIN_BUZZER);  
            reproduciendoMelodia = false;  
        }
    }
}

void llamadaPaciente() {
    paciente_llamo = true;
    reproducirMelodia();
    informarPulsoPaciente();
}

void confirmarLlamada() {
    paciente_llamo = false;
    tone(PIN_BUZZER, TONO_MI, DURACION_BUZZER);
    informarConfirmacion();
}

// Idealmente, una vez implementada la aplicación para celulares,
// estas funciones se encargarían de informar por medio de wifi
// a la aplicación que ocurrió alguno de estos eventos.
// De momento envían un mensaje por medio de Serial
void informarPulsoPaciente() {
    Serial.println("El paciente ha pulsado el botón de llamada");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("El paciente");
    lcd.setCursor(0, 1);
    lcd.print("llama");
}

void informarConfirmacion() {
    Serial.println("Se ha confirmado la llamada del paciente");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Llamada");
    lcd.setCursor(0, 1);
    lcd.print("confirmada");
}

void informarPausaActuadores() {
    Serial.println("Se han pausado los actuadores");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alerta");
    lcd.setCursor(0, 1);
    lcd.print("aplazada");
}

void informarReanudacionActuadores() {
    Serial.println("Se han reanudado los actuadores");
}

void informarLevanto() {
    Serial.println("El paciente se levantó");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("El paciente");
    lcd.setCursor(0, 1);
    lcd.print("se ha levantado");
}

void informarOrino() {
    Serial.println("El paciente se orinó");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("El paciente");
    lcd.setCursor(0, 1);
    lcd.print("ha orinado");
}
