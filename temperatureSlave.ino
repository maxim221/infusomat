#include <Wire.h>

#include <I2C_Anything.h>

#define TEMPERATURE_DEVICE 8

void setup() {
  Wire.begin(TEMPERATURE_DEVICE);
  Wire.onRequest(requestEvent);
}

volatile float temp1;
volatile float temp2;

void requestEvent()
{ 
 temp1 = -102.12;
 temp2 = 100.52;
 
 I2C_singleWriteAnything(temp1);
 I2C_singleWriteAnything(temp2);
}

void loop() {
  // put your main code here, to run repeatedly:

}
