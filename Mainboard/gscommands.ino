#include "constants.h"
void commsUpdate() {

  static char buffer[2] = {0,0};
  static uint8_t bufferPosition = 0;
  if(Serial1.available() > 0) {  //comms e pe Serial1, asculta pe Serial1
    buffer[bufferPosition] = Serial1.read();

    bufferPosition++;
    if(bufferPosition >= 2) {   //check for overflow
      bufferPosition = 0;
    }

    if(buffer[0] == buffer[1]) {

      //everything ok, execute the command, if allowed by system state
      if(armedState == SYSTEM_NOT_ARMED) {
        switch(buffer[0]) {
          //Camera LED control
          case 'L':
            digitalWrite(CAM_LED, HIGH);
            break;
          case 'l':
            digitalWrite(CAM_LED, LOW);
            break;
          
          //Turn ON/OFF camera
          case 'C':
            digitalWrite(CAM_EN, HIGH);
            break;
          case 'c':
            digitalWrite(CAM_EN, LOW);
            break;

          //12V DC/DC converter
          case 'P': //ON
            digitalWrite(ONOFF12,HIGH);
            break;
          case 'p': //OFF
            digitalWrite(ONOFF12,LOW);
            break;

          //10V DC/DC converter
          case 'R': //ON
            digitalWrite(ONOFF10,HIGH);
            break;
          case 'r': //OFF
            digitalWrite(ONOFF10,LOW);
            break;

          //Linear Motor
          case 'S': //retracted
            linearMotor.write(linearMotor_retracted);
            break;
          case 's': //soldering
            linearMotor.write(linearMotor_extended);
            break;

          //Sample disc rotate
          case 'M': //clockwise
            targetStepsDisk += stateData.targetStepsDisk;
            break;
          case 'm': //counterclockwise
            targetStepsDisk -= stateData.targetStepsDisk;
            break;

          //FM rotate
          case 'D': //clockwise
            targetStepsFM += stateData.targetStepsFM;
            break;
          case 'd': //counterclockwise
            targetStepsFM -= stateData.targetStepsFM;
            break;

          //Heating soldering tip
          case 'H': //heating on
            targetTemperature = solderingTemperature;
            break; 
          case 'h': //heating off
            targetTemperature = 0;
            break;

          //ARM system to HOT/COLD (test) runs
          case 'F': //Arm HOT
            armExperiment(SYSTEM_ARMED_HOT);
            break;
          case 'N': //Arm COLD
            armExperiment(SYSTEM_ARMED_COLD);
            break;

          default:
            Serial.print("Unknown command from GS:");
            Serial.println(buffer[0]);
            break;
        }

      }else if((armedState == SYSTEM_ARMED_COLD) || (armedState == SYSTEM_ARMED_HOT))
      {
        //Here we ignore all commands received while we are ARMED HOT/COLD
        //except for disarm
        switch(buffer[0]) {
          case 'x':
            disarmExperiment();
            //Serial.println("Disarmed");
            break;
        }
      }



      //empty the buffer, to avoid multiple execution
      buffer[0] = buffer[1] = 0;

    }else{
      if(bufferPosition == 0) {
        buffer[0] = buffer[1];
        buffer[1] = 0;
        bufferPosition = 1;
      }
    }
  }
}
