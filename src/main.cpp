#include <estado.h>

#define BAUDRATE      9600

void setup() {
  Serial.begin(BAUDRATE);

  Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);  
  lcd.begin(LCD_COLUMNAS, LCD_FILAS);
  lcd.backlight();

  // Configuracion inicial del LCD
  lcd.clear();
  lcd.setCursor(LCD_COLUMNA_INICIAL, LCD_FILA_INICIAL);
  lcd.print("Iniciando");
  lcd.setCursor(0,1);
  lcd.print("SmartCare Alert");

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
  if(nuevo_evento != EV_CONTINUE) //Esto es para evitar spam
    Serial.println("Estado: "+estados_string[estado_actual]+"  Evento: "+eventos_string[nuevo_evento]);

  fsm();
  delay(10);
}
