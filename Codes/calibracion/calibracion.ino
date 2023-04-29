//para hallar el valor de R0 que utlilizaremos en los parametros de los sensores. 
void setup(){
 Serial.begin (9600); 
}
void loop (){
  
  float sensor_volt;
  float RS_air;
  float R0;
  float sensorValue;
  for (int x =0; x<100; x++)
  {
    sensorValue = sensorValue + analogRead(25);
  }
  sensorValue = sensorValue /100;

  sensor_volt = sensorValue /1024 *5.0;
  RS_air = (5.0 - sensor_volt)/sensor_volt;
  R0 = RS_air/27.5; // este valor cambia segun el sensor, el dato lo porvee el datasheet 
  Serial.print ("sensor_volt = ");
  Serial.print (sensor_volt);
  Serial.println("V");

  Serial.print ("R0 =");
  Serial.println (R0,10);
  delay(1000);
}
