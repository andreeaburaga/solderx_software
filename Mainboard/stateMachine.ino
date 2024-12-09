void stateMachineUpdate()
{
  //static unsigned long lastStateChange = 0;
  
  switch (machineState)
  {
    case 0: {
        //data recording
        // data transmission
        //send data to dlu
      }
      break;

    case 1:
      {
        // keep this empty, the experiment starts here
        //lastStateChange = millis();
        digitalWrite(ONOFF12, HIGH);
        digitalWrite(EN_DISK, HIGH);
        digitalWrite(NSLEEP_DISK, HIGH);
        //functie
        digitalWrite(EN_FM, HIGH);
        digitalWrite(NSLEEP_FM, HIGH);
        // Serial.println("S1");
        machineState++;
        break;
      }
    case 2: {
        digitalWrite(ONOFF10, HIGH);
        // Serial.println("S2");
        machineState++;
        break;
      }
    case 3: {
        if (digitalRead(LO) == 0)
        {
          // Serial.println("LO detected.");
          LO_millis = millis();
          //lastStateChange = millis();
          // Serial.println("S3");
          machineState++;
        }
        break;
      }
    case 4: //LO + 50s
      // wait for LO signal
      {
        if (millis() - LO_millis > LO_delta * second ) {
          if (armedState == SYSTEM_ARMED_HOT)
            targetTemperature = solderingTemperature;
          else
            targetTemperature = 0;
          digitalWrite(CAM_LED, HIGH);
          digitalWrite(CAM_EN, HIGH);
          machineState++;
          // Serial.println("S4");
          //
        }
        break;
      }
    case 5: //SOE = LO + 85s
      {
        if (digitalRead(SOE) == 0)
        {
          //lastStateChange = millis();
          Serial.println("S5");
          machineState++;
        }
        break;
      }
    case 6: { //retract SU from parking slot, move disk
        linearMotor.write(linearMotor_retracted);
        targetStepsDisk += 10;
        Serial.println("S6");
        machineState++;
      }
      break;
    case 7: //SLC
      {
        if (sampleNumber < 17) {
          //

          switch (solderState) {
            case SU_ENGAGE: {
                linearMotor.write(linearMotor_extended);
                if (millis() - stateData.enteredAt > stateData.t_SU_ENGAGE) {
                  //
                  solderState = SU_HEAT;
                  stateData.enteredAt = millis();
                  // Serial.println("Moving to SU_HEAT from SU_ENGAGE");
                }
                break;
              }
            case SU_HEAT: {
                if (millis() - stateData.enteredAt > stateData.t_SU_HEAT) {
                  //
                  if (sampleNumber % 2 == 1) { //wick
                    solderState = SU_RETRACT;
                    // Serial.println("Moving to SU_RETRACT from SU_HEAT (wick)");
                  }
                  else { //pad
                    targetStepsFM += stateData.stepsForwardFM ;
                    solderState = SU_SOLDER;
                    // Serial.println("Moving to SU_SOLDER from SU_HEAT (pad)");
                  }
                  stateData.enteredAt = millis();

                }
                break;
              }
            case SU_SOLDER: {
                if (millis() - stateData.enteredAt > stateData.t_SU_SOLDER) {
                  solderState = SU_RETRACT;
                  targetStepsFM -= stateData.stepsBackwardFM;
                  // Serial.println("Moving to SU_RETRACT from SU_SOLDER");
                }

                break;
              }
            case SU_RETRACT: {
                linearMotor.write(linearMotor_retracted);
                if (millis() - stateData.enteredAt > stateData.t_SU_RETRACT) {
                  //
                  solderState = SU_DISK;
                  // Serial.println("Moving to SU_DISK from SU_RETRACT");
                  targetStepsDisk += 10;
                  stateData.enteredAt = millis();

                }
                break;
              }
            case SU_DISK: {
                if (millis() - stateData.enteredAt > stateData.t_SU_DISK) {
                  sampleNumber++;
                  solderState = SU_ENGAGE;
                  stateData.enteredAt = millis();
                  // Serial.println("Moving to SU_ENGAGE from SU_DISK (sample/wick done)");
                }
                break;
              }
          }
        }
        else {
          machineState++;
        }
        break;
      }
    case 8:
      disarmExperiment();
      break;
    default:
      Serial.print("Machine state: "), Serial.println(machineState);
      break;
  }
}

void armExperiment(uint8_t state) {
  if (state != SYSTEM_ARMED_HOT && state != SYSTEM_ARMED_COLD) {
    Serial.println("Invalid arming.");
    //dataLogger.println("Invalid arming.");
    return;
  }
  targetTemperature = 0;
  machineState = 1;
  armedState = state;
}

void disarmExperiment() {
  machineState = 0;
  armedState = SYSTEM_NOT_ARMED;

  digitalWrite(ONOFF12, LOW);
  targetTemperature = 0;
  //ToDo: Return sample disc to initial position. Then the linear motor to park position. Stop feedingMechanism.
}
