//Munich state machine new
#include <Servo.h>
//#include <AccelStepper.h>
//#include <SoftwareSerial.h>
#include "constants.h"
#include "pins.h"

Servo linearMotor;
//#define dataLogger Serial2

unsigned long tStartSample = 0;
uint8_t machineState = 0;
int16_t targetTemperature = 0;
int16_t currentTemperature = 0;

uint8_t armedState = SYSTEM_NOT_ARMED;  

bool DISK_DIR_CW = true; // directie disk clockwise
bool FM_DIR_CW = true;

inline void runTask(int i);

int32_t currentStepsDisk, targetStepsDisk = 0;
int32_t currentStepsFM, targetStepsFM = 0;
//                                    telemetrie_out, temp, fm, disk, blink, telemetrie_in,
unsigned long previousMillis[taskNumber] = {0, 0, 0, 0, 0};

unsigned long deltaMillis[taskNumber] = {500, 50, 30, 30, 500}; 

//TODO: prioritate
unsigned long LO_millis = 0;
unsigned long ms = millis();
bool heatingDone = 0, solderDone = 0, wickDone = 0, move1Done = 0, move2Done = 0;
int sampleNumber = 0;
int sampleDone = 1;
int sampleState = 0;
int valoare_led = 1;

void setup() {
  //DCDC
  pinMode(ONOFF12, OUTPUT);
  pinMode(ONOFF10, OUTPUT);
  digitalWrite(ONOFF12, LOW);
  digitalWrite(ONOFF10, LOW);

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
  linearMotor.write(linearMotor_arm_extended); //departe de disk
  //Kiruna
  digitalWrite(NSLEEP_FM, HIGH);
  digitalWrite(EN_FM, HIGH);
  digitalWrite(DIR_FM, HIGH);

  digitalWrite(NSLEEP_DISK, HIGH);
  digitalWrite(EN_DISK, HIGH);
  digitalWrite(DIR_DISK, HIGH);

  //Camera
  pinMode(CAM_EN, OUTPUT);
  digitalWrite(CAM_EN, LOW);
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
  
}

uint8_t count = 0;

inline void runTask(int i)
{
  switch (i)
  {
    case 0: //telemetrie
      {
        telemetryUpdate();
        break;
      }
    case 1: // temp
      {
        if (count == 0) {
          digitalWrite(PWM_DC, LOW);
        }
        if (count == 1) {
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
    case 2: //disk
      {
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

        if (DISK_DIR_CW)
          currentStepsDisk++;
        else
          currentStepsDisk--;
        break;
      }
    case 3: //fm
     {
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

        if (FM_DIR_CW)
          currentStepsFM++;
        else
          currentStepsFM--;
        break;
      }
    // case 4:  //TODO: de scos
    //   {
    //     if (valoare_led) digitalWrite(CAM_LED, HIGH), valoare_led = 0;
    //     else digitalWrite(CAM_LED, LOW), valoare_led = 1;
    //     break;
    //   }
    case 4:// manual commands
      {
        commsUpdate();
        break;
      }
    default:
      break;
  }
}
