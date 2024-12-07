#define LED A0  //ADC_val0- PIN PC 0
//#define LED A1
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

#define eeprom_1 0
#define SD_PIN_CS 10
File dataFile;
char currentFile[50];

void setup() {
  Serial.begin(38400);
  Wire.begin();
  // startPressureSensor();
  delay(50);
  startTempSensor();
  delay(50);
  Serial.println("dsa");

  // MS5837 Pressure + Temp Sensor
  while (!dlu_sensor.init()) {
    Serial.println("Init failed!");
    //Serial.println("Are SDA/SCL connected correctly?");
    //Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    //Serial.println("\n\n\n");
    delay(1000);
  }

  dlu_sensor.setModel(MS5837::MS5837_30BA);
  dlu_sensor.setFluidDensity(1.225);  // kg/m^3 (freshwater, 1029 for seawater)

  // MPU6050 giroscop etc.
  //   if (!mpu.begin()) {
  //   //Serial.println("Failed to find MPU6050 chip");
  //   while (1) {
  //     delay(10);
  //   }
  // }
  // mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  // mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);


  int idRead = EEPROM.read(eeprom_1) + 1;
  idRead %= 256;
  EEPROM.write(eeprom_1, idRead);
  sprintf(currentFile, "datalog%d.txt\0", idRead);


  if (!SD.begin(SD_PIN_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  //Serial.println("card initialized.");

  //Serial.println(currentFile);
}

void write_to_sd(buffer_hamming hamming_out) {
    int solderingTargetTemperature  = (hamming_out.buf[3] << 8) | hamming_out.buf[2];
    int solderingCurrentTemperature = (hamming_out.buf[5] << 8) | hamming_out.buf[4];
    int sampleDiscPosition          = (hamming_out.buf[7] << 8) | hamming_out.buf[6];
    int feedingMechanismPosition    = (hamming_out.buf[9] << 8) | hamming_out.buf[8];
    int linearMotorPosition         = (hamming_out.buf[11] << 8) | hamming_out.buf[10];
    int LO_State                    = (hamming_out.buf[12] & 1);
    int SOE_State                   = (hamming_out.buf[12] & 2) >> 1;
    int stateMachineStatus          = (hamming_out.buf[13] << 8);

      Serial.println(F("Writing to file"));
      dataFile.println(F("================="));
      dataFile.println(millis());
      dataFile.println(F("Mainboard Data"));
            Serial.println(F("Writing to file2"));


      dataFile.print(F("solderingTargetTemperature:"));
      dataFile.println(solderingTargetTemperature);

      dataFile.print(F("solderingCurrentTemperature:"));
      dataFile.println(solderingCurrentTemperature);
            Serial.println("Writing to file3");

      dataFile.print(F("sampleDiscPosition:"));
      dataFile.println(sampleDiscPosition);
      // delay(50);
      dataFile.print(F("feedingMechanismPosition:"));
      dataFile.println(feedingMechanismPosition);
            Serial.println(F("Writing to file4"));

      dataFile.print(F("linearMotorPosition:"));
      dataFile.println(linearMotorPosition);

      dataFile.print(F("LO_State:"));
      dataFile.println(LO_State);
      // delay(50);

      dataFile.print(F("SOE_State:"));
      dataFile.println(SOE_State);
            Serial.println(F("Writing to file5"));

      // dataFile.print("stateMachineStatus:");
      dataFile.println(stateMachineStatus);

      dataFile.println(F("SensorData"));
            Serial.println(F("Writing to file6"));

      dataFile.print(F("Temp IC3:"));
      dataFile.println(T);
      dataFile.print(F("Temp MPU:"));
      dataFile.println(T);
                  Serial.println(F("Writing to file7"));

      dataFile.println(F("Temp MS5837"));
      dataFile.println(dlu_sensor.temperature());
      dataFile.println(F("Press MS5837"));
      dataFile.println(dlu_sensor.pressure());
      Serial.println(F("Writing to file8"));

}

void loop() {
  // simple temp sensor
  readTempSensor();
  delay(20);

  // MS5837 printing
  dlu_sensor.read();
  delay(20);
  //Serial.println(dlu_sensor.temperature());
  //Serial.println(dlu_sensor.pressure());

  // MPU6050 sensor printing
  // sensors_event_t a, g, temp;
  // mpu.getEvent(&a, &g, &temp);
  // Serial.print("Acceleration X: ");
  // Serial.print(a.acceleration.x);
  // Serial.print(", Y: ");
  // Serial.print(a.acceleration.y);
  // Serial.print(", Z: ");
  // Serial.print(a.acceleration.z);
  // Serial.println(" m/s^2");

  // Serial.print("Rotation X: ");
  // Serial.print(g.gyro.x);
  // Serial.print(", Y: ");
  // Serial.print(g.gyro.y);
  // Serial.print(", Z: ");
  // Serial.print(g.gyro.z);
  // Serial.println(" rad/s");

  // Serial.print("Temperature: ");
  // Serial.print(temp.temperature);
  // Serial.println(" degC");

  char receivedSerial[50] = {0};
      Serial.println(Serial.available());

  if (Serial.available() >= Serial_Bytes) {
    Serial.println("Enough Bytes");

    for (int i = 0; i < Serial_Bytes; i++) {
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

    dataFile = SD.open("test.txt", FILE_WRITE);
    if (dataFile) {
      write_to_sd(hamming_out);
      dataFile.close();
      delay(100);
    } else {
            Serial.print("Error writing to: ");
            Serial.println(currentFile);


      //Serial.print("error opening ");
      //Serial.println(currentFile);
    }
  }

}
