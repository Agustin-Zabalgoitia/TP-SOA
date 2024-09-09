#include <estado.h>

#define BAUDRATE 115200

void setup() {
  Serial.begin(BAUDRATE);

  //Configuración de los pulsadores  
  pulsadorLlamar.pin = PIN_PULSADOR;
  pulsadorAplazar.pin = PIN_PULSADOR_APLAZAR;
  pulsadorConfirmar.pin = PIN_PULSADOR_CONFIRMAR;

  //Configuramos los sensores
  pinMode(PIN_PRESION, INPUT);
  pinMode(PIN_PULSADOR, INPUT);
  pinMode(PIN_PULSADOR_APLAZAR, INPUT);
  pinMode(PIN_PULSADOR_CONFIRMAR, INPUT);
  sensor_humedad.begin(); //El sensor de humedad empieza a funcionar

  //Configuramos los actuadores
  pinMode(PIN_LED_AMARILLO, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
}

void loop() {
  if(nuevo_evento != EV_CONT) //Esto es para evitar spam
    Serial.println("Estado: "+estados_string[estado_actual]+"  Evento: "+eventos_string[nuevo_evento]);

  fsm();
  delay(10); //Según wokwi esto mejora la simulación, hay que sacarlo antes de entregar el tp
}
