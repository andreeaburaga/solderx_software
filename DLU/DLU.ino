#define LED A0 //ADC_val0- PIN PC 0
//#define LED A1
#include <Wire.h>
#include "pressure.h"
#include "temp.h"

void setup() {
  Serial.begin(38400);
  Wire.begin();
  startPressureSensor();
  delay(50);
  startTempSensor();
  delay(50);
}
void loop() {
  readTempSensor();
  delay(50);
  readPressureSensor();
  delay(50);
  char out[50]={};
  sprintf(out,"Temp1:%3.2f, Temp2:%d, Pressure2:%d",T,temperature_MS583,pressure_MS583);
  Serial.println(out);
}
