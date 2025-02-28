#define LED A0 // ADC_val0- PIN PC 0
// #define LED A1
#include <Wire.h>
// #include "pressure.h"
#include "temp.h"
#include <MS5837.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "hamming.h"

#define Serial_Bytes 30

MS5837 dlu_sensor;
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

#define eeprom_1 0
#define SD_PIN_CS 10
File dataFile;
char currentFile[50];
bool ms5837_ok;
bool mpu6050_ok;

void setup()
{
  Serial.begin(38400);
  Wire.begin();
  // startPressureSensor();
  delay(50);
  startTempSensor();
  delay(50);
  Serial.println(F("Temp sensor started"));

  // MS5837 Pressure + Temp Sensor
  if (dlu_sensor.init())
    ms5837_ok = true;
    Serial.println(F("dlu sensor started"));
  if (ms5837_ok)
  {
    dlu_sensor.setModel(MS5837::MS5837_30BA);
    dlu_sensor.setFluidDensity(1.225); // kg/m^3 (freshwater, 1029 for seawater)
    Serial.println(F("dlu sensor okay"));
  }

  // MPU6050 giroscop etc.
  Serial.println(F("MPU6050 start"));
  if (mpu.begin())
  {
    Serial.println(F("MPU6050 okay"));
    mpu6050_ok = true;
  }
  if (mpu6050_ok)
  {
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    /*
    MPU6050_RANGE_2_G
    MPU6050_RANGE_4_G
    MPU6050_RANGE_8_G
    MPU6050_RANGE_16_G
    */
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    /*
    MPU6050_RANGE_250_DEG
    MPU6050_RANGE_500_DEG
    MPU6050_RANGE_1000_DEG
    MPU6050_RANGE_2000_DEG
    */
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    /*
    MPU6050_BAND_260_HZ
    MPU6050_BAND_184_HZ
    MPU6050_BAND_94_HZ
    MPU6050_BAND_44_HZ
    MPU6050_BAND_21_HZ
    MPU6050_BAND_10_HZ
    MPU6050_BAND_5_HZF
    */
  }
  int idRead = EEPROM.read(eeprom_1) + 1;
  idRead %= 256;
  EEPROM.write(eeprom_1, idRead);
  sprintf(currentFile, "datalog%d.txt\0", idRead);

  while (!SD.begin(SD_PIN_CS))
  {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    delay(100);
  }
  // Serial.println("card initialized.");

  // Serial.println(currentFile);
}

void write_to_SD(buffer_hamming hamming_out)
{
  int solderingTargetTemperature = (hamming_out.buf[3] << 8) | hamming_out.buf[2];
  int solderingCurrentTemperature = (hamming_out.buf[5] << 8) | hamming_out.buf[4];
  int sampleDiscPosition = (hamming_out.buf[7] << 8) | hamming_out.buf[6];
  int feedingMechanismPosition = (hamming_out.buf[9] << 8) | hamming_out.buf[8];
  int linearMotorPosition = (hamming_out.buf[11] << 8) | hamming_out.buf[10];
  int LO_State = (hamming_out.buf[12] & 1);
  int SOE_State = (hamming_out.buf[12] & 2) >> 1;
  int stateMachineStatus = (hamming_out.buf[13] << 8);

  // char buf[100] = "\0";
  char startMessage[50] = "\nPackage Start\n";
  // strcat(buf_out, startMessage);
  dataFile.print(startMessage);

  // Serial.println("Writing to file\n");
  // snprintf(buf, sizeof(buf), "Millis:%d\n", millis());
  // strcat(buf_out, buf);

  // snprintf(buf, sizeof(buf), "SolderingTagetTemp:%d\n", solderingTargetTemperature);
  // strcat(buf_out, buf);
  dataFile.print(F("solderingTargetTemperature:"));
  dataFile.println(solderingTargetTemperature);

  // snprintf(buf, sizeof(buf), "SolderingCurrentTemp:%d\n", solderingCurrentTemperature);
  // strcat(buf_out, buf);
  dataFile.print(F("solderingCurrentTemperature:"));
  dataFile.println(solderingCurrentTemperature);

  // snprintf(buf, sizeof(buf), "SampleDiscPos:%d\n", sampleDiscPosition);
  // strcat(buf_out, buf);
  dataFile.print(F("sampleDiscPosition:"));
  dataFile.println(sampleDiscPosition);

  // delay(50);
  // snprintf(buf, sizeof(buf), "FeedingPos:%d\n", feedingMechanismPosition);
  // strcat(buf_out, buf);
  dataFile.print(F("feedingMechanismPosition:"));
  dataFile.println(feedingMechanismPosition);

  // snprintf(buf, sizeof(buf), "LinearMotorPos:%d\n", linearMotorPosition);
  // strcat(buf_out, buf);
  dataFile.print(F("linearMotorPosition:"));
  dataFile.println(linearMotorPosition);

  // snprintf(buf, sizeof(buf), "LO_State:%d\n", LO_State);
  // strcat(buf_out, buf);
  dataFile.print(F("LO_State:"));
  dataFile.println(LO_State);
  // delay(50);

  // snprintf(buf, sizeof(buf), "SOE_State:%d\n", SOE_State);
  // strcat(buf_out, buf);
  dataFile.print(F("SOE_State:"));
  dataFile.println(SOE_State);

  // snprintf(buf, sizeof(buf), "StateMachine:%d\n", stateMachineStatus);
  // strcat(buf_out, buf);
  dataFile.print(F("stateMachineStatus:"));
  dataFile.println(stateMachineStatus);

  // snprintf(buf, sizeof(buf), "\nSensorData:\n");
  // strcat(buf_out, buf);
  dataFile.println(F("SensorData"));
  // Serial.println(F("Writing to file6"));

  // snprintf(buf, sizeof(buf), "Temp IC3:%d\n", T);
  // strcat(buf_out, buf);
  dataFile.print(F("Temp IC3:"));
  dataFile.println(T);

  // snprintf(buf, sizeof(buf), "Temp MS5837:%d\n", dlu_sensor.temperature()); // dlu_sensor.temperature()
  // strcat(buf_out, buf);

  // snprintf(buf, sizeof(buf), "Pressure MS5837:%d\n", dlu_sensor.pressure()); // dlu_sensor.pressure()
  // strcat(buf_out, buf);
  uint8_t ms5837_temp = 255;
  uint8_t ms5837_pressure = 255;
  if (ms5837_ok)
  {
    ms5837_temp = dlu_sensor.temperature();
    ms5837_pressure = dlu_sensor.pressure();
  }
  dataFile.print(F("Temp MS5837:"));
  dataFile.println(ms5837_temp);

  dataFile.print(F("Pressure MS5837:"));
  dataFile.println(ms5837_pressure);

  if (mpu6050_ok)
  {
    dataFile.print(F("Acceleration X: "));
    dataFile.print(a.acceleration.x);
    dataFile.print(F(", Y: "));
    dataFile.print(a.acceleration.y);
    dataFile.print(F(", Z: "));
    dataFile.print(a.acceleration.z);
    dataFile.println(F(" m/s^2"));

    dataFile.print(F("Rotation X: "));
    dataFile.print(g.gyro.x);
    dataFile.print(F(", Y: "));
    dataFile.print(g.gyro.y);
    dataFile.print(F(", Z: "));
    dataFile.print(g.gyro.z);
    dataFile.println(F(" rad/s"));
  }

  Serial.println(F("Package written to SD"));
  dataFile.print(F("millis:"));
  dataFile.println(millis());
  dataFile.println();
}

void loop()
{
  // simple temp sensor
  readTempSensor();
  delay(20);

  // MS5837 printing
  if (ms5837_ok)
    dlu_sensor.read();
  delay(20);

  if (mpu6050_ok){
    mpu.getEvent(&a, &g, &temp);
    Serial.print(F("Acceleration X: "));
    Serial.print(a.acceleration.x);
    Serial.print(F(", Y: "));
    Serial.print(a.acceleration.y);
    Serial.print(F(", Z: "));
    Serial.print(a.acceleration.z);
    Serial.println(F(" m/s^2"));

    Serial.print(F("Rotation X: "));
    Serial.print(g.gyro.x);
    Serial.print(F(", Y: "));
    Serial.print(g.gyro.y);
    Serial.print(F(", Z: "));
    Serial.print(g.gyro.z);
    Serial.println(F(" rad/s"));
  }
  delay(20);

  char receivedSerial[50] = {0};

  if (Serial.available() >= Serial_Bytes)
  {
    Serial.println(F("Enough Bytes"));

    for (int i = 0; i < Serial_Bytes; i++)
    {
      receivedSerial[i] = Serial.read();
    }

    buffer_hamming hamming_in = {
        .buf = receivedSerial,
        .used = 30,
        .allocated = 50,
    };

    char decodedBuffer[60] = {0};
    buffer_hamming hamming_out = {
        .buf = decodedBuffer,
        .used = 0,
        .allocated = 60,
    };
    decode_hamming(&hamming_in, &hamming_out);

    dataFile = SD.open(currentFile, FILE_WRITE);
    if (dataFile)
    {
      write_to_SD(hamming_out);
      Serial.println(F("Se poate scrie"));
      dataFile.close();
      delay(100);
    }
    else
    {
      Serial.print(F("Error writing to: "));
      Serial.println(currentFile);
    }
  }
}
