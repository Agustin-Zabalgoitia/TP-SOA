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
  pinMode(PIN_PULSADOR, INPUT_PULLUP);
  pinMode(PIN_PULSADOR_APLAZAR, INPUT_PULLUP);
  pinMode(PIN_PULSADOR_CONFIRMAR, INPUT_PULLUP);


  //Configuramos los actuadores
  pinMode(PIN_BUZZER, OUTPUT);
  //Configuramos el lcd
  Wire.begin(PIN_SDA, PIN_SCL);
  lcd.begin(LCD_COLUMNAS, LCD_FILAS);
  apagarFondoLCD();
}

void loop() {
  if(nuevo_evento != EV_CONT) //Esto es para evitar spam
    Serial.println("Estado: "+estados_string[estado_actual]+"  Evento: "+eventos_string[nuevo_evento]);

  fsm();
}
