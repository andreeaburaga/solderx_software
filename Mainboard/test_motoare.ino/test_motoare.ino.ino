//Feeding
#define STEP_FM PIN_PA4 //28
#define DIR_FM PIN_PC7 //23
#define EN_FM PIN_PC6 //22
#define NSLEEP_FM PIN_PC5 //21

//Disk
#define STEP_DISK PIN_PC4 //20
#define DIR_DISK PIN_PC3 //19
#define EN_DISK PIN_PC2 //18
#define NSLEEP_DISK PIN_PC1 //17

void setup() {
  // put your setup code here, to run once:
  digitalWrite(STEP_FM,HIGH);
  digitalWrite(NSLEEP_FM,HIGH);
  digitalWrite(EN_FM,HIGH);
  digitalWrite(DIR_FM,HIGH);

  digitalWrite(STEP_DISK,HIGH);
  digitalWrite(NSLEEP_DISK,HIGH);
  digitalWrite(EN_DISK,HIGH);
  digitalWrite(DIR_DISK,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(STEP_DISK,HIGH);
  digitalWrite(STEP_FM,HIGH);
  delayMicroseconds(500);

  digitalWrite(STEP_DISK,LOW);
  digitalWrite(STEP_FM,LOW);
  delayMicroseconds(500);
  

}
