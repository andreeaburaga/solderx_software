#include <Servo.h>
//#include <AccelStepper.h>
//#include <SoftwareSerial.h>
#include "constants.h"
#include "pins.h"

Servo linearMotor;
//AccelStepper sampleDisc(AccelStepper::DRIVER, sampleDisc_stepPin, sampleDisc_dirPin);
//AccelStepper feedingMechanism(AccelStepper::DRIVER, feedingMechanism_stepPin, feedingMechanism_dirPin);
//SoftwareSerial dataLogger(softwareSerial_RXpin, softwareSerial_TXpin); // RX, TX
//#define dataLogger Serial2


unsigned long tStartSample = 0;
uint8_t machineState = 0;
int16_t targetTemperature = 0;
int16_t currentTemperature = 0;

//Variables for heating/reading temperature

bool DISK_DIR_CW = true; // directie disk clockwise
bool FM_DIR_CW = true;

inline void runTask(int i);

int32_t currentStepsDisk = 0, targetStepsDisk = 0;
int32_t currentStepsFM, targetStepsFM = 0;
//                                    telemetrie_out, temp, fm, disk, blink, telemetrie_in,
unsigned long previousMillis[taskNumber] = {0, 0, 0, 0, 0, 0};
unsigned long deltaMillis[taskNumber] = {500, 50, 30, 30, 500, 500};
//TODO: prioritate
unsigned long LO_millis = 0;
unsigned long ms = millis();
bool heatingDone = 0, solderDone = 0, wickDone = 0, move1Done = 0, move2Done = 0;
int sampleNumber = 0;
int sampleDone = 1;
int sampleState = 0;
int valoare_led = 1;
void setup() {
  //Sample disk
  pinMode(STEP_DISK, OUTPUT);
  pinMode(DIR_DISK, OUTPUT);
  pinMode(EN_DISK, OUTPUT);
  pinMode(NSLEEP_DISK, OUTPUT);

  //Feeding mechanism
  pinMode(STEP_FM, OUTPUT);
  pinMode(DIR_FM, OUTPUT);
  pinMode(EN_FM, OUTPUT);
  pinMode(NSLEEP_FM, OUTPUT);


  //Linear
  linearMotor.attach(linearMotor_Pin);
  // NOTE Maria
  linearMotor.write(linearMotor_retracted); //departe de disk


  //  linearMotor.write(linearMotor_retracted);
  //  delay(3000);
  // sampleDisc.setMaxSpeed(100.0);
  // sampleDisc.setAcceleration(50.0);
  // sampleDisc.setCurrentPosition(0);

  // feedingMechanism.setMaxSpeed(100.0);
  // feedingMechanism.setAcceleration(50.0);
  // feedingMechanism.setCurrentPosition(0);

  pinMode(CAM_LED, OUTPUT);
  digitalWrite(CAM_LED, LOW);

  pinMode(PWM_Solder, OUTPUT);
  analogWrite(PWM_Solder, 0);
  //digitalWrite(PWM_Solder, LOW);

  //Setup comms
  Serial.begin(38400);
  // Serial.println("Booting up..");

  Serial1.begin(38400);
  // Serial1.println("Hello Rexus");

  pinMode(LO_Pin, INPUT);
  pinMode(SOE_Pin, INPUT);

  for (int i = 0; i < 4; i++) {
    digitalWrite(CAM_LED, HIGH);
    delay(500);
    digitalWrite(CAM_LED, LOW);
    delay(500);
  }
}

void loop()
{
  ms = millis();

  for (int i = 0; i < taskNumber; i++)
  {
    if (ms - previousMillis[i] >= deltaMillis[i])
    {
      runTask(i);
      previousMillis[i] = ms;
    }
  }
  stateMachineUpdate();
  //  targetStepsFM = 10000;
  //  targetStepsDisk = 10000;
  //  //Serial.println(ms);
}

uint8_t count = 0;

inline void runTask(int i)
{
  switch (i)
  {
    case 0:
      {
        telemetryUpdate();
        // Serial.print("Current Steps FM: "), Serial.println(currentStepsFM);
        // Serial.print("Target Steps FM: "), Serial.println(targetStepsFM);
        // Serial.println();
      }
      break;
    case 1: // temp
    {
      if(count == 0){
        digitalWrite(PWM_DC,LOW);
      }
      if(count == 1){
        calculateTemperature();
        if (currentTemperature < targetTemperature)
          digitalWrite(PWM_DC, HIGH);
        else
          digitalWrite(PWM_DC, LOW);
      }
      count++;
      count = count % 6;
      break;
    }
    case 2:{
        // disk
        //digitalWrite(LED_BUILTIN,HIGH);
        if (currentStepsDisk < targetStepsDisk) {
          DISK_DIR_CW = true;
          digitalWrite(DIR_DISK, HIGH);
        }
        else if (currentStepsDisk > targetStepsDisk) {
          DISK_DIR_CW = false;
          digitalWrite(DIR_DISK, LOW);
        }
        else {
          break;
        }

        digitalWrite(STEP_DISK, HIGH);
        delayMicroseconds(100);    // Control step speed (500us pulse width for 1kHz frequency)
        digitalWrite(STEP_DISK, LOW);
        delayMicroseconds(100);
        // for (int j = 0; j < microsteppingRate; j++) {
        // }

        if (DISK_DIR_CW)
          currentStepsDisk++;
        else
          currentStepsDisk--;
        break;
      }
    case 3: {
        // FM
        //digitalWrite(LED_BUILTIN,HIGH);
        if (currentStepsFM < targetStepsFM) {
          FM_DIR_CW = true;
          digitalWrite(DIR_FM, HIGH);
        }
        else if (currentStepsFM > targetStepsFM) {
          FM_DIR_CW = false;
          digitalWrite(DIR_FM, LOW);
        }
        else {
          break;
        }

        digitalWrite(STEP_FM, HIGH);
        delayMicroseconds(100);    // Control step speed (500us pulse width for 1kHz frequency)
        digitalWrite(STEP_FM, LOW);
        delayMicroseconds(100);
        // for (int j = 0; j < microsteppingRate; j++) {
        // }

        if (FM_DIR_CW)
          currentStepsFM++;
        else
          currentStepsFM--;
        break;
      }
    case 4:
      {
        if (valoare_led) digitalWrite(CAM_LED, HIGH), valoare_led = 0;
        else digitalWrite(CAM_LED, LOW), valoare_led = 1;
        break;
      }
    case 5: {
        commsUpdate();
        break;
      }
    default:
      break;
  }
}
