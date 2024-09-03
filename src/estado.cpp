#include <estado.h>


unsigned short ultimo_indice_lector_sensor = 0;
String estados_string[] = {"ST_MONITOREO", "ST_ORINADO", "ST_LEVANTADO", "ST_PULSADO"};
enum estados estado_actual;
enum estados ultimo_estado;

void get_event()
{
  //leemos los sensores
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

  //Nos fijamos si es válido
    //en caso de serlo actuamos de acuerdo al evento y al estado

  switch ( estado_actual )
  {
    case ST_MONITOREO:
    {
      switch (nuevo_evento)
      {
      case EV_LEVANTO:
        //Al ocurrir un evento hacemos una acción
        Serial.println("El paciente se levantó");//Esto tendría que ser una función a parte

        //Cambiamos el estado
        estado_actual = ST_LEVANTADO;
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
      case EV_CONT:
        digitalWrite(PIN_LED_ROJO, HIGH);//Esto tendría que ser una función a parte
        break;
      
      case EV_TIMEOUT:
        digitalWrite(PIN_LED_ROJO, LOW);//Esto tendría que ser una función a parte
        estado_actual = ST_MONITOREO;
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