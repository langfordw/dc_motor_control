#include "dc_motor.h"

#define encA1_int 0
#define encB1_int 1
#define encA2_int 2
#define encB2_int 3

DCMotor *motor1 = new DCMotor(5, 4, 10, A1, 3, 2, 32); //dir_pin1, dir_pin2, pwm_pin, current_sense_pin, encA_pin, encB_pin, counts_per_revolution
DCMotor *motor2 = new DCMotor(6, 7, 9, A0, 0, 1, 32);

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  motor1->init();
  motor1->setCurrentLimit(0.8); // amps
  motor1->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor1->setPolarity(1);
  
//  motor2->init();
//  motor2->setCurrentLimit(0.8); // amps
//  motor2->setPWMLimit(255); // given nominal 6v motor with 6V power supply
//  motor2->setPolarity(0);
  
//  motor1->setPIDGains(1.5,0.0,0.0); // velocity control
//  motor1->setDesiredVelocity(2000); //counts/sec

  motor1->setPIDGains(2,0.00,0); // position control
//  motor1->setPIDGains(0,0,500.); // pure damper
//  motor1->setDesiredPosition(0);
  motor1->setDesiredForce(40);
}

void loop() {
//  motor1->setDesiredPosition(10000*sin(millis()/1000.));
//  long target_pos = 5000*(millis()/1000.);
//  motor1->setDesiredPosition(target_pos);
  Serial.print("M1: ");
  Serial.print(motor1->measureForce());
//  Serial.print(", ");
//  Serial.print(motor1->calculateVelocity());
  motor1->update();
//  motor1->drive(255);
  Serial.println();
}

int readTensionGauge() {
  return analogRead(A7);
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
