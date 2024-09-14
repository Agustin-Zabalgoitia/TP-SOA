#include <estado.h>


unsigned short ultimo_indice_lector_sensor = 0;
String estados_string[] = {"ST_MONITOREO", "ST_ORINADO", "ST_LEVANTADO", "ST_LLAMADO"};
enum estados estado_actual;
enum estados ultimo_estado;

rgb_lcd lcd;

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
        informarLevanto();

        //Cambiamos el estado
        estado_actual = ST_LEVANTADO;
        break;
      
      case EV_ORINO:
        informarOrino();
        estado_actual = ST_ORINADO;
        break;

      case EV_PULSO:
        llamadaPaciente();
        break;

      case EV_LLAMO:
        estado_actual = ST_LLAMADO;
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
    
    case ST_LEVANTADO:
    {
      switch (nuevo_evento)
      {
      case EV_ORINO:
      case EV_CONT:
        //digitalWrite(PIN_LED_ROJO, HIGH);
        encenderFondoLCD(ROJO);
        break;
      
      case EV_TIMEOUT:
        //digitalWrite(PIN_LED_ROJO, LOW);
        apagarFondoLCD();
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
        //digitalWrite(PIN_LED_AMARILLO, HIGH);
        encenderFondoLCD(AMARILLO);
        break;
      
      case EV_TIMEOUT:
        //digitalWrite(PIN_LED_AMARILLO, LOW);
        apagarFondoLCD();
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
        //digitalWrite(PIN_LED_AZUL, HIGH);
        encenderFondoLCD(AZUL);
        break;
      
      case EV_TIMEOUT:
        //digitalWrite(PIN_LED_AZUL, LOW);
        apagarFondoLCD();
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

void encenderFondoLCD(colores color){
  switch (color)
  {
  case AZUL:
    lcd.setRGB(0,0,255);
    break;
  
  case AMARILLO:
    lcd.setRGB(255,255,0);
    break;

  case ROJO:
    lcd.setRGB(255,0,0);
    break;

  default:
    break;
  }
}

void apagarFondoLCD(){
  lcd.setRGB(0,0,0);
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