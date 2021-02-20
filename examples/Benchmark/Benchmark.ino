#include "muCom.h"


MUCOM_CREATE(Device, Serial, 6, 1);

long timestamps[14] = {0}; //Timestamps for benchmark
uint8_t u8Dummy = 0;
uint16_t u16Dummy = 0;
uint32_t u32Dummy = 0;
uint64_t u64Dummy = 0;
float f32Dummy = 0.0;
double f64Dummy = 0.0;


void DoNothing(uint8_t *data, uint8_t cnt)
{
  (void)data;
  (void)cnt;
}


void setup()
{
  int i;
  int tmpTime;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Initialize serial interface
  Serial.begin(250000);

  //Link variables
  Device.linkVariable(0, &u8Dummy);
  Device.linkVariable(1, &u16Dummy);
  Device.linkVariable(2, &u32Dummy);
  Device.linkVariable(3, &u64Dummy);
  Device.linkVariable(4, &f32Dummy);
  Device.linkVariable(5, &f64Dummy);

  //Link function
  Device.linkFunction(0, DoNothing);

  //Perform benchmark 100 times and add measured times
  for(i = 0; i < 100; i++) 
  {
    // ------- Benchmark writing variables -------
    tmpTime = micros();
    Device.writeByte(0, 0x55);
    timestamps[0] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
    
    tmpTime = micros();
    Device.writeShort(1, 0x5555);
    timestamps[1] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
    
    tmpTime = micros();
    Device.writeLong(2, 0x55555555);
    timestamps[2] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
    
    tmpTime = micros();
    Device.writeLongLong(3, 0x5555555555555555);
    timestamps[3] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
    
    tmpTime = micros();
    Device.writeFloat(4, -666.66);
    timestamps[4] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
    
    tmpTime = micros();
    Device.writeDouble(5, -666.66);
    timestamps[5] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
    
    
    // ------- Benchmark reading variables -------
    tmpTime = micros();
    u8Dummy = Device.readByte(0);
    timestamps[6] += (int)micros() - tmpTime;
    
    tmpTime = micros();
    u16Dummy = Device.readShort(1);
    timestamps[7] += (int)micros() - tmpTime;
    
    tmpTime = micros();
    u32Dummy = Device.readLong(2);
    timestamps[8] += (int)micros() - tmpTime;
    
    tmpTime = micros();
    u64Dummy = Device.readLongLong(3);
    timestamps[9] += (int)micros() - tmpTime;
    
    tmpTime = micros();
    f32Dummy = Device.readFloat(4);
    timestamps[10] += (int)micros() - tmpTime;
	
    tmpTime = micros();
    f32Dummy = Device.readDouble(5);
    timestamps[11] += (int)micros() - tmpTime;
  
    // ------- Benchmark invoking functions -------
    tmpTime = micros();
    Device.invokeFunction(0);
    timestamps[12] += (int)micros() - tmpTime;
    delay(5);
    Device.handle();
  }

  //Calculate average benchmark result
  tmpTime = micros();
  __asm__("nop\n\t"); //Ensure that micros is actually stored in tmpTime
  timestamps[13] = (int)micros() - tmpTime;
  for(i = 0; i < 14; i++)
  {
    timestamps[i] = (timestamps[i] / 100) - timestamps[13];
  }

  digitalWrite(LED_BUILTIN, HIGH);
}


void loop()
{
  //Print results
  Serial.print("writeByte():      "); Serial.println(timestamps[0]);
  Serial.print("writeShort():     "); Serial.println(timestamps[1]);
  Serial.print("writeLong():      "); Serial.println(timestamps[2]);
  Serial.print("writeLongLong():  "); Serial.println(timestamps[3]);
  Serial.print("writeFloat():     "); Serial.println(timestamps[4]);
  Serial.print("writeDouble():    "); Serial.println(timestamps[5]);
  Serial.print("readByte():       "); Serial.println(timestamps[6]);
  Serial.print("readShort():      "); Serial.println(timestamps[7]);
  Serial.print("readLong():       "); Serial.println(timestamps[8]);
  Serial.print("readLongLong():   "); Serial.println(timestamps[9]);
  Serial.print("readFloat():      "); Serial.println(timestamps[10]);
  Serial.print("readDouble():     "); Serial.println(timestamps[11]);
  Serial.print("invokeFunction(): "); Serial.println(timestamps[12]);
  Serial.println();
  
  delay(5000);
}
