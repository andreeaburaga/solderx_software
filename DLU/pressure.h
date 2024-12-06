#ifndef PRESSURE_H
#define PRESSURE_H
#include "Wire.h"
uint8_t pressure_address = 0x76;
uint8_t reset_command = 0x1E;
uint8_t promStartAddress = 0xA0;
uint8_t readADC_command = 0x00;
uint8_t CONVERT_D1_8192 = 0x48;
uint8_t CONVERT_D2_8192 = 0x58;
uint16_t calibrationCoefficients[7];

const uint8_t MS5837_30BA = 0;
const uint8_t MS5837_02BA = 1;
const uint8_t _model = MS5837_30BA;

int32_t temperature_MS583 = 0;
int32_t pressure_MS583 = 0;

void startPressureSensor(){
  Wire.beginTransmission(pressure_address);
  Wire.write(reset_command);
  Wire.endTransmission();
  delay(10);
  for(int i=0;i<7;i++){
    Wire.beginTransmission(pressure_address);
    Wire.write(promStartAddress+i);
    Wire.endTransmission();
    
    delay(10);
    
    Wire.requestFrom(pressure_address,2);
    if(Wire.available()){
      uint8_t buf1 = Wire.read();
      uint8_t buf2 = Wire.read();
      calibrationCoefficients[i] = (buf1 << 8) | buf2 ;
      Serial.print(i),Serial.print(": "),Serial.println(calibrationCoefficients[i]);
    }
  }
  delay(10);  
}
void readPressureSensor(){
  Wire.beginTransmission(pressure_address);
  Wire.write(CONVERT_D1_8192);
  Wire.endTransmission();
  
  delay(20);

  Wire.beginTransmission(pressure_address);
  Wire.write(readADC_command);
  Wire.endTransmission();

  uint32_t D1_pres = 0, D2_temp = 0;
  Wire.requestFrom(pressure_address,3);
  D1_pres = Wire.read();
  D1_pres = (D1_pres<<8) | Wire.read();
  D1_pres = (D1_pres<<8) | Wire.read();

  Wire.beginTransmission(pressure_address);
  Wire.write(CONVERT_D2_8192);
  Wire.endTransmission();
  
  delay(20);

  Wire.requestFrom(pressure_address,3);
  D2_temp = Wire.read();
  D2_temp = (D2_temp<<8) | Wire.read();
  D2_temp = (D2_temp<<8) | Wire.read();

  int32_t dT = 0;
  int64_t SENS = 0;
  int64_t OFF = 0;
  int32_t SENSi = 0;
  int32_t OFFi = 0;
  int32_t Ti = 0;
  int64_t OFF2 = 0;
  int64_t SENS2 = 0;

  dT = D2_temp-uint32_t(calibrationCoefficients[5])*256l;
  if ( _model == MS5837_02BA ) {
    SENS = int64_t(calibrationCoefficients[1])*65536l+(int64_t(calibrationCoefficients[3])*dT)/128l;
    OFF = int64_t(calibrationCoefficients[2])*131072l+(int64_t(calibrationCoefficients[4])*dT)/64l;
    pressure_MS583 = (D1_pres*SENS/(2097152l)-OFF)/(32768l);
  } else {
    SENS = int64_t(calibrationCoefficients[1])*32768l+(int64_t(calibrationCoefficients[3])*dT)/256l;
    OFF = int64_t(calibrationCoefficients[2])*65536l+(int64_t(calibrationCoefficients[4])*dT)/128l;
    pressure_MS583 = (D1_pres*SENS/(2097152l)-OFF)/(8192l);
  }

  // Temp conversion
  temperature_MS583 = 2000l+int64_t(dT)*calibrationCoefficients[6]/8388608LL;

  //Second order compensation
  if ( _model == MS5837_02BA ) {
    if((temperature_MS583/100)<20){         //Low temp
      Ti = (11*int64_t(dT)*int64_t(dT))/(34359738368LL);
      OFFi = (31*(temperature_MS583-2000)*(temperature_MS583-2000))/8;
      SENSi = (63*(temperature_MS583-2000)*(temperature_MS583-2000))/32;
    }
  } else {
    if((temperature_MS583/100)<20){         //Low temp
      Ti = (3*int64_t(dT)*int64_t(dT))/(8589934592LL);
      OFFi = (3*(temperature_MS583-2000)*(temperature_MS583-2000))/2;
      SENSi = (5*(temperature_MS583-2000)*(temperature_MS583-2000))/8;
      if((temperature_MS583/100)<-15){    //Very low temp
        OFFi = OFFi+7*(temperature_MS583+1500l)*(temperature_MS583+1500l);
        SENSi = SENSi+4*(temperature_MS583+1500l)*(temperature_MS583+1500l);
      }
    }
    else if((temperature_MS583/100)>=20){    //High temp
      Ti = 2*(dT*dT)/(137438953472LL);
      OFFi = (1*(temperature_MS583-2000)*(temperature_MS583-2000))/16;
      SENSi = 0;
    }
  }

  OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
  SENS2 = SENS-SENSi;

  temperature_MS583 = (temperature_MS583-Ti);

  if ( _model == MS5837_02BA ) {
    pressure_MS583 = (((D1_pres*SENS2)/2097152l-OFF2)/32768l);
  } else {
    pressure_MS583 = (((D1_pres*SENS2)/2097152l-OFF2)/8192l);
  }
  
}

#endif
