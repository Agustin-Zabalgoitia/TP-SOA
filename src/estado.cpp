#include <estado.h>

unsigned short ultimo_indice_lector_sensor = 0;
//configuración inicial de los estados
String estados_string[] = {"ST_ESTABLE", "ST_PULSADO", "ST_ORINADO", "ST_LEVANTADO", "ST_APLAZADO"};
enum estados estado_actual;
enum estados ultimo_estado;

//Configuración inicial del display
rgb_lcd lcd;
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
  nuevo_evento = EV_CONTINUE;
}

//Finite-State Machine
void fsm()
{
  //Obtenemos un evento
  get_event();

  //Se valida el estado
  //En caso de serlo, actuamos de acuerdo al evento y al estado

  switch ( estado_actual )
  {
    case ST_ESTABLE:
    {
      switch ( nuevo_evento )
      {
        case EV_PULSO:
        {
          actualizarLCD(true, ST_PULSADO);
          llamadaPaciente();
          estado_actual = ST_PULSADO;
        }
        break;

        case EV_ORINO:
        {
          actualizarLCD(true, ST_ORINADO);
          informarOrino();
          estado_actual = ST_ORINADO;
        }
        break;

        case EV_LEVANTO:
        {
          actualizarLCD(true, ST_LEVANTADO);
          informarLevanto();
          estado_actual = ST_LEVANTADO;
        }
        break;
        
        case EV_CONTINUE:
        {
          actualizarLCD(false, ST_ESTABLE);
        }
        break;

        default:
        break;
      }
    }
    break;

    case ST_PULSADO:
    {
      switch ( nuevo_evento )
      {
        case EV_CONFIRMO:
        {
          actualizarLCD(true, ST_ESTABLE);
          confirmarLlamada();
          estado_actual = ST_ESTABLE;
        }
        break;

        case EV_APLAZO:
        {
          actualizarLCD(true, ST_APLAZADO);
          pausarActuadores();
          ultimo_estado = ST_PULSADO;
          estado_actual = ST_APLAZADO;
        }
        break;

        case EV_ORINO:
        {
          actualizarLCD(true, ST_ORINADO);
          informarOrino();
          estado_actual = ST_ORINADO;
        }
        break;

        case EV_LEVANTO:
        {
          actualizarLCD(true, ST_LEVANTADO);
          informarLevanto();
          estado_actual = ST_LEVANTADO;
        }
        break;
        
        case EV_CONTINUE:
        {
          actualizarLCD(false, ST_PULSADO);
        }
        break;

        default:
        break;
        }
    }
    break;

    case ST_ORINADO:
    {
      switch ( nuevo_evento )
      {
        case EV_CONFIRMO:
        {
          actualizarLCD(true, ST_ESTABLE);
          confirmarLlamada();
          estado_actual = ST_ESTABLE;
        }
        break;

        case EV_APLAZO:
        {
          actualizarLCD(true, ST_APLAZADO);
          pausarActuadores();
          ultimo_estado = ST_ORINADO;
          estado_actual = ST_APLAZADO;
        }
        break;

        case EV_LEVANTO:
        {
          actualizarLCD(true, ST_LEVANTADO);
          informarLevanto();
          estado_actual = ST_LEVANTADO;
        }
        break;
        
        case EV_CONTINUE:
        {
          actualizarLCD(false, ST_ORINADO);
        }
        break;

        default:
        break;
        }
    }
    break;
    
    case ST_LEVANTADO:
    {
      switch ( nuevo_evento )
      {
        case EV_CONFIRMO:
        {
          actualizarLCD(true, ST_ESTABLE);
          confirmarLlamada();
          estado_actual = ST_ESTABLE;
        }
        break;
        
        case EV_CONTINUE:
        {
          actualizarLCD(false, ST_LEVANTADO);
        }
        break;

        default:
        break;
        }
    }
    break;

    case ST_APLAZADO:
    {
      switch ( nuevo_evento )
      {
        case EV_CONFIRMO:
        {
          actualizarLCD(true, ST_ESTABLE);
          confirmarLlamada();
          estado_actual = ST_ESTABLE;
        }
        break;

        case EV_LEVANTO:
        {
          actualizarLCD(true, ST_LEVANTADO);
          informarLevanto();
          estado_actual = ST_LEVANTADO;
        }
        break;

        case EV_TIMEOUT:
        {
          actualizarLCD(true, ultimo_estado);
          estado_actual = ultimo_estado;
        }
        break;
        
        case EV_CONTINUE:
        {
          actualizarLCD(false, ST_APLAZADO);
        }
        break;

        default:
        break;
        }
    }
    break;

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
  tone(PIN_BUZZER, TONO_SI, DURACION_BUZZER);
  informarPulsoPaciente();
}

void confirmarLlamada()
{
  tone(PIN_BUZZER, TONO_MI, DURACION_BUZZER);
  informarConfirmacion();
}

void actualizarLCD(bool forzar, estados estado)
{
    /*
      Esta función imprime por una pantalla LCD el estado en el que se encuentra el SE.
      La impresión por pantalla se realiza según un intervalo de tiempo definido por la constante "TIEMPO_ESCRITURA_LCD".
      El parámetro "forzar" nos permite realizar la impresión aunque no se haya completado el intervalo de tiempo.
      El parámetro "estado" nos permite indicar el estado que debe ser impreso.
    */

    unsigned long tiempo = millis();

    if(!forzar && tiempo - tiempo_lcd < TIEMPO_ESCRITURA_LCD)
    {
      return;
    }

    tiempo_lcd = tiempo;
    lcd.clear();

    switch ( estado )
    {
      case ST_ESTABLE:
      {
        lcd.print("Paciente ");
        lcd.setCursor(0,1);
        lcd.print("estable");
      }
      break;
        
      case ST_PULSADO:
      {
        lcd.print("Paciente ");
        lcd.setCursor(0,1);
        lcd.print("llamando!");
      }
      break;

      case ST_ORINADO:
      {
        lcd.print("Paciente ");
        lcd.setCursor(0,1);
        lcd.print("orinado!");
      }
      break;

      case ST_LEVANTADO:
      {
        lcd.print("Paciente ");
        lcd.setCursor(0,1);
        lcd.print("levanto!");
      }
      break;

      case ST_APLAZADO:
      {
        lcd.print("Aplazado");
      }
      break;

      default:
      break;
    }
    
}

/*
  Las siguientes funciones notifican a través de la consola la situación.
  Estas notificaciones serán enviadas al celular una vez implementada la aplicación Android.
*/

void informarPulsoPaciente()
{
  Serial.println("El paciente ha pulsado el botón de llamada.");
}

void informarOrino()
{
  Serial.println("El paciente se ha orinado.");
}

void informarLevanto()
{
  Serial.println("El paciente se ha levantado.");
}

void informarConfirmacion()
{
  Serial.println("Se ha confirmado la llamada del paciente.");
}

void informarPausaActuadores()
{
  Serial.println("Se han pausado los actuadores."); //Esto todavía no ocurre
}