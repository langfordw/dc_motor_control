#include "dc_motor.h"

#define encA1_int 3
#define encB1_int 2
#define encA2_int 0
#define encB2_int 1

DCMotor *motor1 = new DCMotor(5, 4, 10, A1, 1, 0, 1000);
DCMotor *motor2 = new DCMotor(6, 7, 9, A0, 3, 2, 1000);

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  motor1->init();
  motor1->setCurrentLimit(0.8); // amps
  motor1->setPWMLimit(100); // given nominal 6v motor with 15v power supply
  motor1->setPolarity(0);
  
  motor2->init();
  motor2->setCurrentLimit(0.8); // amps
  motor2->setPWMLimit(100); // given nominal 6v motor with 15v power supply
  motor2->setPolarity(0);
  
//  motor1->setDesiredVelocity(10); //rpm
}

void loop() {
  motor1->update();
  motor2->update();
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
