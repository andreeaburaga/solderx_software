#include "hamming.h"
void telemetryUpdate() {

  // PROASPAT ADAUGAT
  static int i = 0;
  i++;
  if (i % 2 == 0) {
    digitalWrite(CAM_LED, HIGH);
  } else {
    digitalWrite(CAM_LED, LOW);
  }

  //
  //  static unsigned long lastTelemetrySent = 0;
  //  unsigned long timeNow = millis();
  //
  //  //Send new telemetry at 2 Hz
  //  if(timeNow - lastTelemetrySent >= 500) {
  //    lastTelemetrySent = timeNow;
  //
  char telemetryMessage[15];
  telemetryMessage[0] = 0b10101010;
  telemetryMessage[1] = 0b10101010;
  telemetryMessage[2] = targetTemperature & 0xFF;
  telemetryMessage[3] = (targetTemperature >> 8) & 0xFF;
  telemetryMessage[4] = currentTemperature & 0xFF;
  telemetryMessage[5] = (currentTemperature >> 8) & 0xFF;

  //uint16_t sampleDiscPosition = abs(currentStepsDisk);
  telemetryMessage[6] = currentStepsDisk & 0xFF;
  telemetryMessage[7] = (currentStepsDisk >> 8) & 0xFF;



  uint16_t feedingMechanismPosition = abs(currentStepsFM);
  telemetryMessage[8] = feedingMechanismPosition & 0xFF;
  telemetryMessage[9] = (feedingMechanismPosition >> 8) & 0xFF;

  uint16_t linearMotorPosition = linearMotor.read();
  telemetryMessage[10] = linearMotorPosition & 0xFF;
  telemetryMessage[11] = (linearMotorPosition >> 8) & 0xFF;

  //Rexus signals (LO and SOE)
  telemetryMessage[12] = digitalRead(LO_Pin) + (digitalRead(SOE_Pin) << 1);

  //Machine state
  telemetryMessage[13] = machineState;

  //calculate and add checksum
  uint8_t checksum = 0;
  for (int i = 2; i < 14; i++) {
    checksum ^= telemetryMessage[i];
  }
  telemetryMessage[14] = checksum;

  buffer_hamming input = {
    .buf = telemetryMessage,
    .used = 15,
    .allocated = 15
  };

  char buf[30];
  buffer_hamming output = {
    .buf = buf,
    .used = 0,
    .allocated = 30
  };

  encode_hamming(&input,&output);
  Serial1.write(output.buf, 30);
  // Serial1.write("Hello World tele\n");


  //dataLogger.write(telemetryMessage, 14);
}
