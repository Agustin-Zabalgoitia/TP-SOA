//Ejemplo obtenido de https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino
/*
#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Initialize DHT sensor.
DHT sensor_humedad(DHTPIN, DHTTYPE);
void setup() {
  sensor_humedad.begin();
}

void leerHumedad() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humedad = sensor_humedad.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humedad)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(humedad);
}
*/