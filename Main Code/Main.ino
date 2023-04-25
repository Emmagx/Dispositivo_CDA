//Definir Librerias a Usar
#include <WiFi.h>
#include <ThingSpeak.h>
#include <MQUnifiedsensor.h>
#include <WiFiManager.h>
#include "DHT.h"

//Declaramos las credenciales wifi por Defecto
const char* ssid="BITMEC_PRODUCCION";
const char* password="Produccion22*";

//Definimos los Pin y e tipo de sensor DHT: DHT11 o DHT22
#define DHTPIN 14
#define DHTTYPE DHT11
//Se define el sensor DHT
DHT dht(DHTPIN, DHTTYPE);

//Definimos las credenciales para la conexión a la plataforma
//Agregamos el ID del Canal al que nos queremos comunicar
unsigned long channelID = 2118820; //Numero/Nombre del canal
//Definimos la Api Key que vamos a usar.
const char* WriteAPIKey="4WPMV3E0BN7F1B2P"; 

//Se definen todos valores de los sensores MQ.
//Definicion    Nombre Variable         Valor
#define         Board                   ("ESP32") //Definimos la tarjeta en una variable a usar: ESP32, ESP8622, ARDUINO 1, ARDUINO Nano...
#define         MQ7Pin                  (4)  //Analog input, definimos en una variable el Pin que vamos a usar en el Sensor MQ-7 
#define         MQ7Type                 ("MQ-7") //Definimos el sensor quee estara en ese Pin Definido.
#define         MQ5Pin                  (25) //Analog input, definimos en una variable el Pin que vamos a usar en el Sensor MQ-5 
#define         MQ5Type                 ("MQ-5") //Definimos el sensor quee estara en ese Pin Definido.
#define         MQ4Pin                  (21) //Analog input, definimos en una variable el Pin que vamos a usar en el Sensor MQ-4
#define         MQ4Type                 ("MQ-4") //Definimos el sensor quee estara en ese Pin Definido.
#define         Voltage_Resolution      (5) //Voltaje que usaremos: 5v, 3.3v
#define         ADC_Bit_Resolution      (12) //Cantidad de bits de informacion que tolera nuestra tarjeta. Ver documentacion individual de la tarjeta 
#define         RatioMQ7CleanAir        (27.5)//Valor de lectura en aire "limpio" del sensor MQ-7
#define         RatioMQ5CleanAir        (6.5) //Valor de lectura en aire "limpio" del sensor MQ-5
#define         RatioMQ4CleanAir        (4.4) //Valor de lectura en aire "limpio" del sensor MQ-4

//Declaramos Sensores
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ7Pin, MQ7Type); //sensor MQ-7
MQUnifiedsensor MQ5(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ5Pin, MQ5Type); //sensor MQ-5
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ4Pin, MQ4Type); //sensor MQ-4
//DECLARAMOS las variables que usaremos en tipo flotante(Decimal), sin valor definido
float dioxido; //Variable que almacenera el dato del Dioxido
float humedad; //Variable que almacenera el dato del Humedad
float metano;  //Variable que almacenera el dato del Metano
float propano; //Variable que almacenera el dato del Propano
float temperatura; //Variable que almacenera el dato del Temperatura
//Variables para la funcion MILIS
unsigned long lastTime = 0; // variable para almacenar el tiempo de la última lectura
const unsigned long interval = 5000; // intervalo de tiempo entre lecturas (en milisegundos)
//Se inicializa el servicio de WiFi en modo cliente.
WiFiClient cliente;
//Codigo que se almacenara lo que se hara en la inicializacion de la tarjeta.
void setup() {
  WiFi.mode(WIFI_STA);  //Colocamos el modo Wifi a modo estacion
  Serial.begin(115200); //Colocamos en que frecuencia se va a leer el monitor serial

  WiFiManager wiFiManager; //Revisa las credenciales de WIFI, 
  //wiFiManager.resetSettings(); //resetea los datos de las redes wifi antes conocidas
  bool res = wiFiManager.autoConnect("Dispositivo", "12345678"); //inicia un servidor web para poder conectarnos a la placa y asi manejar las credenciales de WiFi
  //prueba la conexion a internet
  if (!res) {
    Serial.println("Failed to connect!");// sino se pudo conectar
  } else {
    Serial.println("Connected :)");     //Si se encuentra conectado
  }
//Conectamos a thinkspeak
  ThingSpeak.begin(cliente);
  delay(5000); //Esperamos 5 segundos para darle tiempo para conectarse
//inicializamos el sensor DHT
  dht.begin();

  //CO Parametros
  MQ7.setRegressionMethod(1); //Partes por Millon, como quiero que se presenten los datos.
  /*
  GAS     | a      | b
  H2      | 69.014  | -1.374
  LPG     | 700000000 | -7.703
  CH4     | 60000000000000 | -10.54
  CO      | 99.042 | -1.518
  Alcohol | 40000000000000000 | -12.35
  */
  MQ7.setA(99.042); MQ7.setB(-1.518); // Valor depende del GAS a leer
  MQ7.setR0(0.2868324816); // Valor encontrado al analizar el sensor con el codigo de calibración.

  //C3H8 Parametros
  MQ5.setRegressionMethod(1); //Partes por Millon, como quiero que se presenten los datos.
  /*
  Exponential regression:
  Gas    | a      | b
  H2     | 1163.8 | -3.874
  LPG    | 80.897 | -2.431
  CH4    | 177.65 | -2.56
  CO     | 491204 | -5.826
  Alcohol| 97124  | -4.918
  */
  MQ5.setA(80.897); MQ5.setB( -2.4312); // A Y B depende del gas a Leer.
  MQ5.setR0(3.0120439529); //Valor encontrado al analizar el sensor con el codigo de calibración.

  //LDP Parametros
  MQ4.setRegressionMethod(1); //Partes por Millon, como quiero que se presenten los datos.
    /*
    Exponential regression:
  Gas    | a      | b
  H2     | 1163.8 | -3.874
  LPG    | 80.897 | -2.431
  CH4    | 177.65 | -2.56
  CO     | 491204 | -5.826
  Alcohol| 97124  | -4.918
  */
  MQ4.setA(3811.9); MQ4.setB(-3.113); //// A Y B depende del gas a Leer.
  MQ4.setR0(1.7213886976);
  // Con el Metodo de Regresion 1:
  //*********Esta es la formula que utiliza la libreria para dar los datos: PPM =  a*ratio^b *****************
}
//Aqui empieza el codigo Principal.
void loop() {
  //Iniciamos la Lectura del Sensor DHT, con una funcion Milis, que dejara seguir usando los demas sensores y cada 5 segundos lea la temperatura.
  unsigned long currentTime = millis(); //Le damos este valor a la variable que nos va a ayudar a mantener la funcion
    if (currentTime - lastTime >= interval) { //empezamos la funcion con un parametro que cuando iguale al intervalo va a realizarse.
    lastTime = currentTime; //iguala ambas variables

    humedad = dht.readHumidity(); //Leemos humedad con una funcion ya definida por la libreria
    temperatura = dht.readTemperature(); //Leemos Temperatura con una funcion ya definida por la libreria
    //Imprimimos valores en el monitor serial, para poder verlos en caso de no tener conexion a nuestra nube IOT
    Serial.print("temperatura: "); // Imprimimos temperatura y sus datos.
    Serial.print(temperatura);
    Serial.println(" °C");
    Serial.print("Humedad: "); //Imprimimos Humedad y sus datos.
    Serial.print(humedad);
    Serial.print("%");}

  MQ7.init(); //Inicializamos sensor MQ7
  MQ7.update(); //Llama funciones de la libreria
  MQ7.readSensor(); //Lee el sensor con la formula dada anteriormente
  dioxido = MQ7.readSensor(); //Le da el valor a la variable dioxido el de la lectura
  Serial.print("CO: "); // Imprime datos de lecturas
  Serial.println(dioxido,10); // Imprime el valor con 10 decimales
  delay(1000); //Damos una separación de un segundo con el Otro sensor

  MQ5.init(); //Inicializamos Sensor MQ5
  MQ5.update(); //Llama funciones de la Libreria.
  MQ5.readSensor(); //Llama a la funcion de leer sensor
  propano = MQ5.readSensor(); //Le da el valor a la variable propano de la lectura
  Serial.print("GAS: "); //Imprime los datos En el monitor Serial
  Serial.println(propano, 10); //Imprime los valores con 10 decimales.
  delay(1000); //Damos una separación de un segundo con el Otro sensor

  MQ4.init(); //Incializamos sensor MQ4
  MQ4.update(); //Llamamos a todas las funciones
  MQ4.readSensor(); //Llamamos La funcion que lee el sensor
  metano = MQ4.readSensor(); //Le damos a la variable Metano el valor leido
  Serial.print("LDP: "); //Imprimimos en el monitor serial Los valores encontrados
  Serial.println(metano,20); //Imprimimos el valor del metano con 20 decimales.
  delay(1000); //Agregamos una pausa de un segundo para la siguiente funcion.

//Hacemos la conexión y envío de datos a la plataforma, utilizando las credenciales definidas anteriormente
  ThingSpeak.writeFields(channelID,WriteAPIKey);
//Imprimimos una frase indicando el envío, y agregamos un retardo de 10 segundos*/
  Serial.println("Datos enviados a ThingSpeak!");
  delay(10000);  
//Indicamos el orden de envío por campos o Field, en el orden definido de la plataforma, junto a los valores del sensor
//Función             Campo, Nombre de la variable que rellenará el campo
  ThingSpeak.setField(1,temperatura); //como leer esta parte del codigo: "Mando al campo 1 de Things speak el valor de la variable Temperatura"
  ThingSpeak.setField(2,humedad); //como leer esta parte del codigo: "Mando al campo 2 de Things speak el valor de la variable Humedad"
  ThingSpeak.setField(3,metano);  //como leer esta parte del codigo: "Mando al campo 3 de Things speak el valor de la variable Metano"
  ThingSpeak.setField(4,propano); //como leer esta parte del codigo: "Mando al campo 4 de Things speak el valor de la variable Propano"
  ThingSpeak.setField(5,dioxido); //como leer esta parte del codigo: "Mando al campo 5 de Things speak el valor de la variable Dioxido"
}
