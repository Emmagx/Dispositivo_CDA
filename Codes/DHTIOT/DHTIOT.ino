/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/cf57b7c7-16a6-4b38-8dbe-dc204d866dc8 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float dioxido;
  float humedad;
  float metano;
  float propano;
  float temperatura;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.println("Prueba de conexion a internet");
  Serial.begin(9600);
  delay(1500); 
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  dht.begin();
}
void loop() {
  ArduinoCloud.update();
  delay(2000);
  float humedad = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperatura = dht.readTemperature();
  Serial.print("temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.print("%");   
  
}
