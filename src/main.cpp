#include <estado.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include <ArduinoJson.h>

// #define DESACTIVATE '0' 
// #define ACTIVATE    '1'


// const char* ssid        = "SO Avanzados";
// const char* password    = "SOA.2019";
// const char* mqttServer  = "broker.emqx.io";
// const char* user_name   = "";
// const char* user_pass   = "";

// const char * topic_temp  = "/casa/temperatura";
// const char * topic_luz = "/casa/luz";

// int port = 1883;
// String stMac;
// char mac[50];
// char clientId[50];
// long last_time= millis();

// WiFiClient espClient;
// PubSubClient client(espClient);

#define BAUDRATE      9600

void setup() {
  Serial.begin(BAUDRATE);

  Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);  
  lcd.begin(LCD_COLUMNAS, LCD_FILAS);

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
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(PIN_PULSADOR, INPUT_PULLUP);
  pinMode(PIN_PULSADOR_APLAZAR, INPUT_PULLUP);
  pinMode(PIN_PULSADOR_CONFIRMAR, INPUT_PULLUP);

  //Configuramos los actuadores
  pinMode(PIN_BUZZER, OUTPUT);

  Serial.println("Entra al Setup");
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  wifiConnect();

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  stMac = WiFi.macAddress();
  stMac.replace(":", "_");
  Serial.println(stMac);
  
  client.setServer(mqttServer, port);
  client.setCallback(callback);
  pinMode(2, OUTPUT);

  Serial.println("print2");

  Serial.println("Fin del setup");

}

void wifiConnect()  
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
}

//Funcion que reconecta el cliente, si pierde la conexion
void mqttReconnect() 
{
    if(!client.connected())
    {
      Serial.print("Intentando conexión MQTT...");
      long r = random(1000);
      sprintf(clientId, "clientId-%ld", r);
      if (client.connect(clientId,user_name,user_pass)) 
	  {
        Serial.print(clientId);
        Serial.println(" conectado");
        Serial.println("envio");
        // client.subscribe("/smartcare/pulso");
        // client.subscribe("/smartcare/orino");
        // client.subscribe("/smartcare/levanto");
        client.subscribe("/smartcare/aplazo");
      } else 
	  {
        Serial.print("fallo, rc=");
        Serial.print(client.state());
        Serial.println("intentando de nuevo en 5 segundos");
        delay(5000);
      }
  }
}

//Funcion Callback que recibe los mensajes enviados por lo dispositivos
// void callback(char* topic, byte* message, unsigned int length) 
// {
//   char cMessage=char(*message);

//   Serial.print("Se recibio mensaje en el topico: ");
//   Serial.println(topic);
//   Serial.print("Mensaje Recibido: ");
//   Serial.println(cMessage);
//   Serial.println();
  
//   if(cMessage== ACTIVATE)
//     digitalWrite(2, HIGH);
//   else
//     digitalWrite(2, LOW);

// }

String generateJson()
{
    JsonDocument doc;
    String json;
    
    json="";

    doc["value"]=String(random(50));   
    serializeJson(doc,json) ;           

    return json;
}

void loop() {
  if(nuevo_evento != EV_CONTINUE) //Esto es para evitar spam
    Serial.println("Estado: "+estados_string[estado_actual]+"  Evento: "+eventos_string[nuevo_evento]);

  fsm();

  String json;
  char charMsgToSend[3];

  delay(10);
  if (!client.connected()) 
  {
    mqttReconnect();
  }

  
  long now = millis();
  if (now - last_time > 10000) 
  {
    
    json=generateJson();
    json.toCharArray(charMsgToSend, json.length()+1);
	
	//Se publica un mensaje en un topico del broker
    client.publish(topic_temp,(const char *)charMsgToSend);

    Serial.println("envio a Broker: ");
    Serial.println(charMsgToSend);
    Serial.println("");
    
    last_time = now;
  }

  client.loop();

  delay(10);

} 