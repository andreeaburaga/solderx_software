#ifndef TEMP_H
#define TEMP_H
uint8_t TEMP_I2C_ADDRESS = (0x40);
uint8_t TEMP_RESET = 0x1E;
uint8_t TEMP_START_CONVERSION = (0x46);
uint8_t TEMP_READ_SERIAL_NUMBER = (0x0A);
uint8_t TEMP_READ_ADC_val = 0x00;

uint32_t D0, D1, checksum;
float T = 0;


void startTempSensor(){
  Wire.beginTransmission(TEMP_I2C_ADDRESS);
  Wire.write(TEMP_RESET);
  int err = Wire.endTransmission();
  delay(100);
  // Serial.println(err);
  Wire.beginTransmission(TEMP_I2C_ADDRESS);
  Wire.write(TEMP_READ_SERIAL_NUMBER);
  err = Wire.endTransmission();
  // Serial.println(err);
  delay(100);
  int cnt = Wire.requestFrom(TEMP_I2C_ADDRESS, 4);
  // Serial.println(cnt);
  uint8_t serialNumber[4] = {};
  if (Wire.available() <= 4) {
    for (int i = 0; i < 4; i++) {
      serialNumber[i] = Wire.read();
      // Serial.print(serialNumber[i]);
      // Serial.print(' ');
    }
    // Serial.println();
  }
}

void readTempSensor(){
  Wire.beginTransmission(TEMP_I2C_ADDRESS);
  Wire.write(TEMP_START_CONVERSION);
  Wire.endTransmission();
  delay(100);
  
  Wire.beginTransmission(TEMP_I2C_ADDRESS);
  Wire.write(TEMP_READ_ADC_val);
  Wire.endTransmission();
  delay(20);
  
  Wire.requestFrom(TEMP_I2C_ADDRESS, 3);
  D0 = 0, D1 = 0, checksum = 0;
  if ( Wire.available() <= 3 ) {
    D0 = Wire.read();
    D1 = Wire.read();
    checksum = Wire.read();
  }
  delay(20);
  
  uint32_t ADC_val = 0;
  ADC_val = (D0 << 8) + D1;

  double ADC_val_f = (double) ADC_val;

  ADC_val_f *= 165;

  ADC_val_f /= 0xFFFF;
  T = -40.0 + ADC_val_f;

  
}
#endif
