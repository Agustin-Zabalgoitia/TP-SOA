#include <estado.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BAUDRATE 115200

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(BAUDRATE);
    Wire.begin(21, 22);  
    lcd.begin(16, 2);
    lcd.backlight();

    // Configuracion inicial del LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iniciando SE");

    // Configuración de los pulsadores  
    pulsadorLlamar.pin = PIN_PULSADOR; // Es del paciente
    pulsadorAplazar.pin = PIN_PULSADOR_APLAZAR; // Es del enfermero
    pulsadorConfirmar.pin = PIN_PULSADOR_CONFIRMAR; // Es del enfermero

    // Configuramos los sensores
    pinMode(PIN_PRESION, INPUT);
    pinMode(PIN_PULSADOR, INPUT);
    pinMode(PIN_PULSADOR_APLAZAR, INPUT);
    pinMode(PIN_PULSADOR_CONFIRMAR, INPUT);
    sensor_humedad.begin();

    // Configuramos los actuadores
    pinMode(PIN_LED_AMARILLO, OUTPUT);
    pinMode(PIN_LED_AZUL, OUTPUT);
    pinMode(PIN_LED_ROJO, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
}

void loop() {
    if(nuevo_evento != EV_CONT) // Esto es para evitar spam
        Serial.println("Estado: "+estados_string[estado_actual]+"  Evento: "+eventos_string[nuevo_evento]);

    fsm(); 
    delay(10); // Según wokwi esto mejora la simulación, hay que sacarlo antes de entregar el tp
}
