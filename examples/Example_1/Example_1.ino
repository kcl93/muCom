#include "muCom.h"


//Function headers
void setLED(uint8_t *data, uint8_t cnt);


//Create muCom interface
muCom Device(Serial, 2, 1);


//Global variables
//Both will be linked to the muCom interface and can be accessed from the communication partner
int   InputA_LSB = 0;
float InputA_Voltage = 0;


void setup()
{
  //Set LED pin to output
  pinMode(LED_BUILTIN, OUTPUT);

  //Link variables and functions
  Device.linkVariable(0, &InputA_LSB);
  Device.linkVariable(1, &InputA_Voltage);
  Device.linkFunction(0, setLED);

  //Enable serial interface
  Serial.begin(115200);
}


void loop()
{
  //Sample pin and calculate voltage in mV
  InputA_LSB = analogRead(A0);
  InputA_Voltage = (float)InputA_LSB * 5.0 / 1024.0;

  //Print input voltage from the other communication partner
  Serial.print(Device.readFloat(3));
  Serial.println("mV");
  
  //Handle muCom interface
  Device.handle();
}


void setLED(uint8_t *data, uint8_t cnt)
{
  if(cnt == 1)
  {
    digitalWrite(LED_BUILTIN, data[0]);
  }
}
