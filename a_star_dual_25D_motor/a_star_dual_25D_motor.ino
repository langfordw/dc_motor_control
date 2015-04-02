#include "dc_motor.h"

#define encA1_int 0
#define encB1_int 1
#define step_int 2
#define dir_int 3

DCMotor *motor1 = new DCMotor(5, 4, 10, A1, 3, 2, 32); //dir_pin1, dir_pin2, pwm_pin, current_sense_pin, encA_pin, encB_pin, counts_per_revolution

int step_pin = 0;
int dir_pin = 1;

float vel;
float alpha = 0.5;

volatile long desired_position;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(step_int, stepTrig, RISING);
  
  pinMode(step_pin, INPUT);
  digitalWrite(step_pin, HIGH);
  pinMode(dir_pin, INPUT);
  digitalWrite(dir_pin, HIGH);
  
  motor1->init();
  motor1->setCurrentLimit(0.8); // amps
  motor1->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor1->setPolarity(0);

//  motor1->setPIDGains(10.,0.01,10.0); // position control, good for moderate speeds
  motor1->setPIDGains(50.,0.001,500.0); // position control, good for holding stationary
  motor1->setDesiredPosition(0);
}

void loop() {
  // Gain Scheduling Test
  // update gains relative to velocity
  float vel_raw = motor1->calculateVelocity();
  vel = vel*alpha + (1-alpha)*vel_raw;
  float p = (50-vel*40/1000.);
  float d = 500-vel*400/1000.;
  motor1->setPIDGains(p,0.001,d);
  
  motor1->setDesiredPosition(desired_position);
  Serial.print("M1: ");
  Serial.print(desired_position);
  Serial.print(", ");
  Serial.print(motor1->getPosition());
  Serial.print(", ");
  Serial.print(vel);
  motor1->update();
  Serial.println();
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void stepTrig() {
  if (digitalRead(dir_pin)) {
    desired_position++;
  } else {
    desired_position--;
  }
}
