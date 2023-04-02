//Include the library
#include <MQUnifiedsensor.h>
/************************Hardware **************************************************/
#define         Board                   ("ESP32")
#define         Pin                     (4)  //Analog input 4 of your arduino
/***********************Software ***************************************************/
#define         Type                    ("MQ-7") //MQ7
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (12) 
#define         RatioMQ7CleanAir        (27.5) 
/*****************************Globals***********************************************/
//Declare Sensor
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);
void setup (){
  Serial.begin(9600);
   MQ7.setRegressionMethod(1); //_PPM =  a*ratio^b
   MQ7.setA(99.042); MQ7.setB(-1.518); 
   MQ7.setR0(0.2868324816);
}
void loop (){
MQ7.init();
MQ7.update();
float ppm = MQ7.readSensor();
Serial.print("CO: ");
Serial.println(ppm,10);
delay(1000);

}
