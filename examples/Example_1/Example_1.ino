#include "muCom.h"


muCom Device(Serial, 10, 10);
int time;
int diff;


void setup()
{
  Serial.begin(250000);

  diff = micros();
  diff = micros() - diff;
}


void loop()
{
  time = micros();
  Device.writeLong(5, 0x12345678);
  time = micros() - time - diff;
  delay(1000);
  Serial.println();
  Serial.println(time);
  Serial.println();
  delay(1000);
  time = micros();
  Serial.print(5);
  Serial.print(",");
  Serial.println(0x12345678);
  time = micros() - time - diff;
  delay(1000);
  Serial.println();
  Serial.println(time);
  Serial.println();
  delay(1000);
}
