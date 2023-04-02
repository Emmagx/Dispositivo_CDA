#include <MQUnifiedsensor.h>
/************************Hardware *************************************************/
#define         Board                   ("ESP32")
#define         Pin                     (4)  //Analog input 4 of your arduino
/***********************Software *************************************************/
#define         Type                    ("MQ-5") //MQ5
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (12) // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (6.5) //RS / R0 = 60 ppm 
/*****************************Globals***********************************************/
//Declare Sensor
MQUnifiedsensor MQ5(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);
void setup (){
  Serial.begin(9600);
   MQ5.setRegressionMethod(1); //_PPM =  a*ratio^b
   MQ5.setA(80.897); MQ5.setB( -2.4312); 
   MQ5.setR0(3.0120439529); 
}
void loop (){
MQ5.init();
MQ5.update();
float ppm = MQ5.readSensor();
Serial.print("GAS: ");
Serial.println(ppm,10);
delay(1000);

}
