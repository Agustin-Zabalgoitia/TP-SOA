#include <evento.h>
#include <estado.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

unsigned long ultimo_tiempo_actual = 0;
unsigned long tiempo_lectura_presion = 0;
unsigned long tiempo_lectura_humedad = 0;
unsigned long tiempo_evento_llamada = 0;
unsigned long tiempo_evento_timeout = 0;
unsigned long tiempo_evento_timeout_aplazo = 0;
bool paciente_llamo = false;
bool aplazado = false;

String eventos_string[] = {"EV_CONT", "EV_ORINO", "EV_LEVANTO", "EV_PULSO", "EV_LLAMO", "EV_APLAZO", "EV_CONFIRMAR", "EV_TIMEOUT", "EV_BOTON_CONFIRMAR"};

lectorSensor lector_sensor[] = {sensar_presion, sensar_humedad, consultar_llamada, sensar_llamada, sensar_aplazo, sensar_confirmacion, consultar_timeout, consultar_timeout_aplazo};

enum eventos nuevo_evento;
enum eventos ultimo_evento;

DHT sensor_humedad(PIN_HUMEDAD, DHTYPE);

pulsador pulsadorLlamar;
pulsador pulsadorAplazar;
pulsador pulsadorConfirmar;

bool consultar_llamada(bool forzar, unsigned long tiempo_actual) {
    if (tiempo_actual == 0)
        tiempo_actual = millis();

    unsigned long diferencia = (forzar) ? (TIEMPO_LEER_SENSORES) : (tiempo_actual - tiempo_evento_llamada);

    if (diferencia >= TIEMPO_LEER_SENSORES) {
        tiempo_evento_llamada = tiempo_actual;

        if (paciente_llamo) {
            nuevo_evento = EV_LLAMO;
            return true;
        }
    }

    return false;
}

bool sensar_humedad(bool forzar, unsigned long tiempo_actual) {
    if (tiempo_actual == 0)
        tiempo_actual = millis();

    unsigned long diferencia = (forzar) ? (TIEMPO_LEER_SENSORES) : (tiempo_actual - tiempo_lectura_humedad);

    if (diferencia >= TIEMPO_LEER_SENSORES) {
        tiempo_lectura_humedad = tiempo_actual;

        unsigned short valor_lectura = sensor_humedad.readHumidity(); // Lee la humedad

        if (valor_lectura >= UMBRAL_HUMEDAD) {
            nuevo_evento = EV_ORINO;
            return true;
        }
    }
    return false;
}

bool sensar_presion(bool forzar, unsigned long tiempo_actual) {
    if (tiempo_actual == 0)
        tiempo_actual = millis();

    unsigned long diferencia = (forzar) ? (TIEMPO_LEER_SENSORES) : (tiempo_actual - tiempo_lectura_presion);

    if (diferencia >= TIEMPO_LEER_SENSORES) {
        tiempo_lectura_presion = tiempo_actual;

        unsigned short valor_lectura = analogRead(PIN_PRESION);

        if (valor_lectura <= UMBRAL_PRESION) {
            nuevo_evento = EV_LEVANTO;
            return true;
        }
    }
    return false;
}

bool sensar_confirmacion(bool forzar, unsigned long tiempo_actual) {
    return sensar_pulsador(&pulsadorConfirmar, EV_CONFIRMAR);
}

bool sensar_aplazo(bool forzar, unsigned long tiempo_actual) {
    return sensar_pulsador(&pulsadorAplazar, EV_APLAZO);
}

bool sensar_llamada(bool forzar, unsigned long tiempo_actual) {
    return sensar_pulsador(&pulsadorLlamar, EV_PULSO);
}

bool sensar_pulsador(pulsador *pulsador, eventos evento) {
    pulsador->estado_actual = digitalRead(pulsador->pin);
    bool cambio = (pulsador->estado_actual == HIGH && pulsador->estado_anterior == LOW);

    if (cambio) {
        nuevo_evento = evento;
    }
    pulsador->estado_anterior = pulsador->estado_actual;
  
    return cambio;
}

bool consultar_timeout(bool forzar, unsigned long tiempo_actual) {
    if (tiempo_actual == 0)
        tiempo_actual = millis();

    unsigned long diferencia = (forzar) ? (TIEMPO_LEER_SENSORES) : (tiempo_actual - tiempo_evento_timeout);

    if (diferencia >= TIEMPO_TIMEOUT) {
        tiempo_evento_timeout = tiempo_actual;
        nuevo_evento = EV_TIMEOUT; 
        return true;
    }

    return false;
}

bool consultar_timeout_aplazo(bool forzar, unsigned long tiempo_actual) {
    if (tiempo_actual == 0)
        tiempo_actual = millis();

    unsigned long diferencia = tiempo_actual - tiempo_evento_timeout_aplazo;

    if (aplazado && (diferencia >= TIEMPO_APLAZO)) {
        if (ultimo_estado == ST_ORINADO) {
            nuevo_evento = EV_TIMEOUT_APLAZO_ORINADO;
        } else if (ultimo_estado == ST_LLAMADO) {
            nuevo_evento = EV_TIMEOUT_APLAZO_PULSADO;
        }
        aplazado = false;  // Reseteamos después de manejar el timeout
        return true;
    }
    return false;
}

void apagarTodosLosLEDs() {
    digitalWrite(PIN_LED_AMARILLO, LOW);  
    digitalWrite(PIN_LED_AZUL, LOW);     
    digitalWrite(PIN_LED_ROJO, LOW);     
}
