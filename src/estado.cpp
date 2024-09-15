#include <estado.h>

unsigned short ultimo_indice_lector_sensor = 0;
//configuración inicial de los estados
String estados_string[] = {"ST_MONITOREO", "ST_ORINADO", "ST_LEVANTADO", "ST_LLAMADO"};
enum estados estado_actual;
enum estados ultimo_estado;

//Configuración inical del display
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNAS, LCD_FILAS);
unsigned long tiempo_lcd = 0; //temporizador del lcd

void get_event()
{
  //Leemos los sensores
  unsigned short indice = 0;
  unsigned long tiempo_actual = millis();
  unsigned long diferencia = (tiempo_actual - ultimo_tiempo_actual);
  bool timeout = (diferencia > TIEMPO_ENTRE_CICLOS_DE_EJECUCION)?(true):(false);

  if(timeout)
  {
    timeout = false;
    ultimo_tiempo_actual = tiempo_actual;

    indice = (ultimo_indice_lector_sensor % MAX_LECTORES);
    ultimo_indice_lector_sensor++;

    if(lector_sensor[indice](false, tiempo_actual))
      return; //Si los sensores detectaron un evento salimos para atenderlo en fsm
  }
  //Si los sensores no detectaron un nuevo evento, continuamos
  nuevo_evento = EV_CONT;
}

//Finite-State Machine
void fsm()
{
  //Obtenemos un evento
  get_event();

  //Nos fijamos si el estado es válido
    //en caso de serlo actuamos de acuerdo al evento y al estado

  switch ( estado_actual )
  {
    case ST_MONITOREO:
    {
      switch (nuevo_evento)
      {
      case EV_LEVANTO:
        //Al ocurrir un evento hacemos una acción
        //Fuerza la actualización del lcd para que indique que pasamos al estado levantado
        actualizarLCD(true, ST_LEVANTADO);
        informarLevanto();

        //Cambiamos el estado
        estado_actual = ST_LEVANTADO;
        break;
      
      case EV_ORINO:
        //Fuerza la actualización del lcd para que indique que pasamos al estado orinado
        actualizarLCD(true, ST_ORINADO);
        informarOrino();
        estado_actual = ST_ORINADO;
        break;

      case EV_PULSO:
        llamadaPaciente();
        break;

      case EV_LLAMO:
        //Fuerza la actualización del lcd para que indique que pasamos al estado llamado
        actualizarLCD(true, ST_LLAMADO);
        estado_actual = ST_LLAMADO;
        break;

      case EV_APLAZO:
        pausarActuadores();
        break;

      case EV_CONFIRMAR:
        confirmarLlamada();
        break;

      case EV_CONT:
        //Actualiza el lcd para informar que estamos en el estado de monitoreo
        actualizarLCD(false, ST_MONITOREO); 

      default:
        break;
      }
    }
    break;
    
    case ST_LEVANTADO:
    {
      switch (nuevo_evento)
      {
      case EV_ORINO:
      case EV_CONT:
        digitalWrite(PIN_LED_ROJO, HIGH);
        break;
      
      case EV_TIMEOUT:
        digitalWrite(PIN_LED_ROJO, LOW);
        estado_actual = ST_MONITOREO;
        break;

      case EV_APLAZO:
        pausarActuadores();
        break;

      case EV_CONFIRMAR:
        confirmarLlamada();
        break;

      default:
        break;
      }
    }
    break;

    case ST_ORINADO:
    {
      switch (nuevo_evento)
      {
      case EV_CONT:
        digitalWrite(PIN_LED_AMARILLO, HIGH);
        break;
      
      case EV_TIMEOUT:
        digitalWrite(PIN_LED_AMARILLO, LOW);
        estado_actual = ST_MONITOREO;
        break;

      case EV_APLAZO:
        pausarActuadores();
        break;

      case EV_CONFIRMAR:
        confirmarLlamada();
        break;

      default:
        break;
      }
    }
    break;

    case ST_LLAMADO:
    {
      switch (nuevo_evento)
      {
      case EV_CONT:
        digitalWrite(PIN_LED_AZUL, HIGH);
        break;
      
      case EV_TIMEOUT:
        digitalWrite(PIN_LED_AZUL, LOW);
        estado_actual = ST_MONITOREO;
        break;

      case EV_APLAZO:
        pausarActuadores();
        break;

      case EV_CONFIRMAR:
        confirmarLlamada();
        break;

      default:
        break;
      }
    }

    default:
      break;
  }

}

//Definición de funciones propias
void pausarActuadores(){
  tone(PIN_BUZZER, TONO_SOL, DURACION_BUZZER);
  informarPausaActuadores();
}

void llamadaPaciente()
{
  paciente_llamo = true;
  tone(PIN_BUZZER, TONO_SI, DURACION_BUZZER);
  informarPulsoPaciente();
}

void confirmarLlamada()
{
  paciente_llamo = false;
  tone(PIN_BUZZER, TONO_MI, DURACION_BUZZER);
  informarConfirmacion();
}

void actualizarLCD(bool forzar, estados estado)
{
    unsigned long tiempo = millis();

    if(forzar || tiempo - tiempo_lcd > TIEMPO_ESCRITURA_LCD)
    {
        tiempo_lcd = tiempo;
        lcd.clear();

        switch (estado)
        {
        case ST_MONITOREO:
            lcd.print("Monitoreando...");
            break;
        
        case ST_LLAMADO:
            lcd.print("Paciente");
            lcd.setCursor(0,1);
            lcd.print("llamo!");
            break;

        case ST_ORINADO:
            lcd.print("Paciente se");
            lcd.setCursor(0,1);
            lcd.print("orino!");
            break;

        case ST_LEVANTADO:
            lcd.print("Paciente se");
            lcd.setCursor(0,1);
            lcd.print("levanto!");
            break;

        default:
            break;
        }
    }
}

//Idealmente, una vez implementada la aplicación para celulares,
//estas funciones se encargarían de informar por medio de wifi
//a la aplicación que ocurrió alguno de estos eventos.
//De momento envían un mensaje por medio de Serial
void informarPulsoPaciente()
{
  Serial.println("El paciente ha pulsado el botón de llamada");
}

void informarConfirmacion()
{
  Serial.println("Se ha confirmado la llamada del paciente");
}

void informarPausaActuadores()
{
  Serial.println("Se han pausado los actuadores"); //Esto todavía no ocurre
}

void informarLevanto()
{
  Serial.println("El paciente se levantó");
}

void informarOrino()
{
  Serial.println("El paciente se orinó");
}