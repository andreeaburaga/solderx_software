#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "pins.h"

const uint32_t second = 1000UL;
#define solderingTemperature 300
#define taskNumber 6

#define LO_delta 5 //seconds

#define microsteppingRate 128
#define STEP_PERIOD 10 // micros , de vazut cum e cu microstepping


enum solder_sm{
  SU_ENGAGE, SU_HEAT, SU_SOLDER, SU_RETRACT, SU_DISK //SU_SOLDER DOAR PE SAMPLENUMBER PAR
};
solder_sm solderState;

struct solder_sm_data{
  unsigned long enteredAt=0;
  const unsigned long t_SU_ENGAGE = 2 * second;
  const unsigned long t_SU_HEAT = 2 * second;
  const unsigned long t_SU_SOLDER = 2 * second;
  const unsigned long t_SU_RETRACT = 3 * second;
  const unsigned long t_SU_DISK = 1 * second;
  const int stepsForwardFM = 30;
  const int stepsBackwardFM = 10;
  
} stateData;

//Motors
const uint8_t linearMotor_Pin = PWM_MOTOR;
const uint8_t linearMotor_extended = 84;   //motor is soldering 6 units/mm
const uint8_t linearMotor_retracted =  72;   //motor is retracted

const uint8_t sampleDisc_stepPin = STEP_DISK;
const uint8_t sampleDisc_dirPin = DIR_DISK;

const uint8_t feedingMechanism_stepPin = STEP_FM;
const uint8_t feedingMechanism_dirPin = DIR_FM;

const uint8_t PWM_Solder = PWM_DC; //heating thermocouple

//simple pins
//const uint8_t cameraLed_Pin = CAM_LED;
//const uint8_t DCDCconverter10V_Pin = ONOFF10;
//const uint8_t DCDCconverter12V_Pin = ONOFF12;

//Rexus signals
const uint8_t LO_Pin = LO;
const uint8_t SOE_Pin = SOE;

//SoftwareSerial needs RX and TX pins
const uint8_t softwareSerial_RXpin = RXD1;
const uint8_t softwareSerial_TXpin = TXD1;


//State machine
#define SYSTEM_NOT_ARMED    0   //Allows TCs from GND
#define SYSTEM_ARMED_COLD   1   //SolderingUnit will stay cold during experiment run
#define SYSTEM_ARMED_HOT    2   //SolderingUnit will stay heatup during experiment run
uint8_t armedState = SYSTEM_NOT_ARMED;  

#endif
