#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastTime = 0; // variable para almacenar el tiempo de la última lectura
const unsigned long interval = 5000; // intervalo de tiempo entre lecturas (en milisegundos)  
void setup() {
  dht.begin();
}

void loop() {
    // leer el sensor DHT11 sólo si ha pasado el intervalo de tiempo configurado
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    
    int humedad = dht.readHumidity();
    int temperatura = dht.readTemperature();
    
    /*Serial.print("temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.print("%");*/
  }

}
