#include "dc_motor.h"

#define encA1_int 0
#define encB1_int 1
#define encA2_int 2
#define encB2_int 3

DCMotor *motor1 = new DCMotor(5, 4, 10, A1, 3, 2, 32); //dir_pin1, dir_pin2, pwm_pin, current_sense_pin, encA_pin, encB_pin, counts_per_revolution
DCMotor *motor2 = new DCMotor(6, 7, 9, A0, 0, 1, 32);

long t_start = micros();
long pos = 0;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  motor1->init();
  motor1->setCurrentLimit(0.8); // amps
  motor1->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor1->setPolarity(1);

  motor1->setPIDGains(1.,0.00,20.); // position control
//  motor1->setDesiredForce(40);
//  motor1->setDesiredVelocity(20);
}

void loop() {
  if (Serial.available()) {
    if (Serial.read() == 'g') {
      //go
      while(1) {
  Serial.print("M1: ");
//  Serial.print(motor1->measureForce());
  Serial.print(motor1->getPosition());
  if (micros() > t_start + 500) {
    pos++;
    motor1->setDesiredPosition(pos);
    t_start = micros();
  }
  Serial.print(", ");
  Serial.print(pos);
  motor1->update();
  Serial.println();
      }
    }
  }
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
