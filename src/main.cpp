#include <estado.h>

#define BAUDRATE 9600

void setup() {
  Serial.begin(BAUDRATE);

  //Configuración de los pulsadores  
  pulsadorLlamar.pin = PIN_PULSADOR;
  pulsadorAplazar.pin = PIN_PULSADOR_APLAZAR;
  pulsadorConfirmar.pin = PIN_PULSADOR_CONFIRMAR;

  //Configuramos los sensores
  pinMode(PIN_PRESION, INPUT);
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(PIN_PULSADOR, INPUT);
  pinMode(PIN_PULSADOR_APLAZAR, INPUT);
  pinMode(PIN_PULSADOR_CONFIRMAR, INPUT);


  //Configuramos los actuadores
  pinMode(PIN_BUZZER, OUTPUT);
  //Configuramos el lcd
  Wire.begin(21, 22);
  lcd.begin(16, 2);
}

void loop() {
  Serial.println(analogRead(PIN_HUMEDAD));
  /*
  if(nuevo_evento != EV_CONT) //Esto es para evitar spam
    Serial.println("Estado: "+estados_string[estado_actual]+"  Evento: "+eventos_string[nuevo_evento]);
  */
  fsm();
  delay(10); //Según wokwi esto mejora la simulación, hay que sacarlo antes de entregar el tp
}
