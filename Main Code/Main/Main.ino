//Este codigo esta separado por Funciones, y todas son llamadas en el setup o en el LOOP

//Definimos las librerias
#include <ThingSpeak.h> //Libreria Oficial de ThingsPeak que nos permite comunicarnos con la NUBE
#include <MQUnifiedsensor.h> //Libreria de todos los Sensores MQ
#include <WiFiManager.h> //Libreria que nos permite abrir un servidor web para conectar a conexiones wifi.
#include "DHT.h" //Libreria para usar el modulo DHT


//Declaramos las credenciales wifi por Defecto
const char* ssid = ""; // El SSID es el nombre de tu RED WIFI, o sea, depende de tu casa. Esto se define en la pestana arduino_secrets.h 
const char* password = ""; // Contraseña de tu WIFI o del WIfi al que lo quieras conectar por defecto. Esto se define en la pestana arduino_secrets.h 

//Definimos los Pin y e tipo de sensor DHT: DHT11 o DHT22
#define DHTPIN 32 //Solo se pueden usar estos pin 32, 33, 34, 35, 36, 37, 38 y 39. Ya que al conectar el ESP32 a Wifi no permite leer otros pin de manera analogica 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);//Creamos un objeto con las propiedades dadas anteriormente.

//Definimos las credenciales para la conexión a la plataforma
unsigned long channelID = 2118820; //Numero/Nombre del canal
const char* WriteAPIKey="4WPMV3E0BN7F1B2P"; //Definimos la Api Key que vamos a usar.

//Se definen todos valores de los sensores MQ.
//En los pines, solo se pueden utilizar estos 32, 33, 34, 35, 36, 37, 38 y 39. Ya que al conectar el ESP32 a Wifi no permite leer otros pin de manera analogica
//!!!!!!!!!!!IMPORTANTE: Un pin por Sensor!!!!!!!!!!!!!1

//Definicion    Nombre Variable         Valor
#define         Board                   ("ESP32") //Definimos la tarjeta en una variable a usar: ESP32, ESP8622, ARDUINO 1, ARDUINO Nano...
#define         MQ7Pin                  (33)  //Analog input, definimos en una variable el Pin que vamos a usar en el Sensor MQ-7 
#define         MQ7Type                 ("MQ-7") //Definimos el sensor quee estara en ese Pin Definido.
#define         MQ5Pin                  (34) //Analog input, definimos en una variable el Pin que vamos a usar en el Sensor MQ-5 
#define         MQ5Type                 ("MQ-5") //Definimos el sensor quee estara en ese Pin Definido.
#define         MQ4Pin                  (35) //Analog input, definimos en una variable el Pin que vamos a usar en el Sensor MQ-4
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

//Funcion que nos conecta a Thinkspeak.
void connectToThingSpeak() {
  ThingSpeak.begin(cliente); //Iniciamos el servicio Thinkspeak con el WIFI cliente que definimos anteriormente
  delay(5000); // Espera 5 segundos para establecer la conexion.
}

//Funcion que inicializa todos los sensores
void initializeSensors() {
  dht.begin(); //Inicializa DHT
  MQ7.init();  //Inicializa MQ7
  MQ5.init();  //Inicializa MQ5
  MQ7.init();  //Inicializa MQ4
  
  MQ7.setRegressionMethod(1); // Forma de como vamos a leer los datos 1= Forma exponencial. PPM
  /*
  Exponential regression:
  GAS     | a      | b
  H2      | 69.014  | -1.374
  LPG     | 700000000 | -7.703
  CH4     | 60000000000000 | -10.54
  CO      | 99.042 | -1.518
  Alcohol | 40000000000000000 | -12.35
  */
  MQ7.setA(99.042);  MQ7.setB(-1.518);  //Datos brindados por el Datasheet, depende del gas a leer por cada sensor.
  MQ7.setR0(0.2868324816); //Valor que encontramos al calibrar los sensores. 

  MQ5.setRegressionMethod(1); // Forma de como vamos a leer los datos 1= Forma exponencial. PPM
  /*
  Exponential regression:
  Gas    | a      | b
  H2     | 1163.8 | -3.874
  LPG    | 80.897 | -2.431
  CH4    | 177.65 | -2.56
  CO     | 491204 | -5.826
  Alcohol| 97124  | -4.918
  */
  MQ5.setA(80.897);  MQ5.setB(-2.4312); //Datos brindados por el Datasheet, depende del gas a leer por cada sensor.
  MQ5.setR0(3.0120439529); //Valor que encontramos al calibrar los sensores. 
  
  MQ4.setRegressionMethod(1); // Forma de como vamos a leer los datos 1= Forma exponencial. PPM
  /*
  Exponential regression:
  Gas    | a      | b
  LPG    | 3811.9 | -3.113
  CH4    | 1012.7 | -2.786
  CO     | 200000000000000 | -19.05
  Alcohol| 60000000000 | -14.01
  smoke  | 30000000 | -8.308
  */
  MQ4.setA(1012.7);  MQ4.setB(-2.786);  //Datos brindados por el Datasheet, depende del gas a leer por cada sensor.
  MQ4.setR0(1.7213886976); //Valor que encontramos al calibrar los sensores. 
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi..");
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);

  WiFiManager wiFiManager;
  //wiFiManager.resetSettings();
  bool res = wiFiManager.autoConnect("Dispositivo", "12345678"); //este es el nombre y la contraseña que se le da a donde nos conectaremos para darle una red wifi
  //prueba la conexion a internet
  if (!res) {
    Serial.println("Failed to connect!");
  } else {
    Serial.println("Connected :)");
  }
}

void setup() {
  Serial.begin(115200); //Iniciamos el monitor serial a 115200 
  //Llamamos a las funciones que definimos anteriormente
  connectToWiFi(); 
  connectToThingSpeak();
  initializeSensors();
}
/*Funciones que se usaran en el LOOP*/

//Funcion que lee el sensor DHT
void leerSensorDHT() {
  //Leemos cada 5s el sensor DHT sin interrumpir la lectura de los demas sensores
  //unsigned long currentTime = millis(); //Iguala los tiempos
  //if (currentTime - lastTime >= interval) { //Entra en un bucle si el ultimo tiempo y el ultimo tiempo registrado siguen siendo mayores al intervalo
    //lastTime = currentTime;
    //humedad = dht.readHumidity(); //Lee el sensor DHT y le da su valor a la variable humedad
    //temperatura = dht.readTemperature(); //lee el sensor DHT y le da su valor a la variable temperatura.
    /*Imprimimos los datos en el monitor Serial*/
    //Serial.print("temperatura: "); 
    //Serial.print(temperatura);
    //Serial.println(" °C");
    //Serial.print("Humedad: ");
    //Serial.print(humedad);
    //Serial.print("%");
  //}
}

//Funcion que lee el sensor MQ7
void leerSensorMQ7() {
  MQ7.update(); //Llamamos la funcion update
  dioxido = MQ7.readSensor(); //le damos el valor que leímos a la variable dioxido
  //Imprimimos los valores en el monitor Serial
  Serial.print("CO: ");
  Serial.println(dioxido,10); 
  delay(1000); //damos un segundo de delay para la siguiente medición
}

//Funcion que lee el sensor MQ5
void leerSensorMQ5() {
  MQ5.update(); //Llamamos la funcion update
  propano = MQ5.readSensor(); //le damos el valor que leímos a la variable propano
  //Imprimimos los valores en el monitor Serial
  //Serial.print("GAS: ");
  //Serial.println(propano, 10);
  //delay(1000);//damos un segundo de delay para la siguiente medición
}

//Funcion que lee el sensor MQ5
void leerSensorMQ4() {
  MQ4.update(); //Llamamos la funcion update
  metano = MQ4.readSensor(); //le damos el valor que leímos a la variable metano
  //Imprimimos los valores en el monitor Serial
  //Serial.print("LDP: ");
  //Serial.println(metano,20); 
  //delay(1000);//damos un segundo de delay para la siguiente medición
}
//Función que envia los datos a la NUBE
void enviarDatosThingSpeak() {
  ThingSpeak.writeFields(channelID,WriteAPIKey); //Nos subscribimos a la nube con el ID y el API ya definimos arriba
  Serial.println("Datos enviados a ThingSpeak!"); 
  delay(10);
  ThingSpeak.setField(1,temperatura); //Llenamos el campo 1 en thingspeak con el valor temperatura, el numero depende del campo a llenar y lo acompaña la variable con el dato a llenar.
  ThingSpeak.setField(2,humedad);     //Llenamos el campo 2 en thingspeak con el valor humedad, el numero depende del campo a llenar y lo acompaña la variable con el dato a llenar.
  ThingSpeak.setField(3,metano);      //Llenamos el campo 3 en thingspeak con el valor metano, el numero depende del campo a llenar y lo acompaña la variable con el dato a llenar.
  ThingSpeak.setField(4,propano);     //Llenamos el campo 4 en thingspeak con el valor propano, el numero depende del campo a llenar y lo acompaña la variable con el dato a llenar.
  ThingSpeak.setField(5,dioxido);     //Llenamos el campo 5 en thingspeak con el valor dioxido, el numero depende del campo a llenar y lo acompaña la variable con el dato a llenar.
}

//Llamamos a las funciones ya definidas anteriormente.
void loop() {
  leerSensorDHT();
  leerSensorMQ7();
  leerSensorMQ5();
  leerSensorMQ4();
  enviarDatosThingSpeak();
}
