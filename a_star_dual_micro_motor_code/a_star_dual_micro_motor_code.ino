#include "dc_motor.h"

// BE SURE TO SET THE CORRECT INTERRUPT CORRESPONDING TO THE DIGITAL PIN OF THE ENCODER
#define encA1_int 0
#define encB1_int 1
#define encA2_int 2
#define encB2_int 3

// 11 and 12 are dummy pins that aren't actually used
DCMotor *motor1 = new DCMotor(4, 11, 10, A1, 3, 2, 10); //dir_pin1, dir_pin2, pwm_pin, current_sense_pin, encA_pin, encB_pin, counts_per_revolution
DCMotor *motor2 = new DCMotor(5, 12, 9, A0, 1, 0, 10);
// CW = NEGATIVE
// CCW = POSITIVE


int stop_state = 1;
int go_state = 0;
int state = 0;

byte activate_pin1 = 7;
byte activate_pin2 = 8;
byte led_pin = 13;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  motor1->init();
  motor1->setCurrentLimit(0.1); // amps
  motor1->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor1->setPolarity(1);
  
  motor2->init();
  motor2->setCurrentLimit(0.1); // amps
  motor2->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor2->setPolarity(1);
  
//  motor1->setPIDGains(1.5,0.0,0.0); // velocity control
//  motor1->setDesiredVelocity(2000); //counts/sec

  motor1->setPIDGains(10.0,0.00,0); // position control
//  motor1->setPIDGains(0,0,500.); // pure damper
//  motor1->setDesiredPosition(0);
}

void loop() {
  if (Serial.available() > 0) {
    if (Serial.read() == 's') {
      state = stop_state;
    }
    if (Serial.read() == 'g') {
      state = 0;
      digitalWrite(led_pin, HIGH);
    }
  }
//  motor1->setDesiredPosition(10000*sin(millis()/1000.));
//  long target_pos = 5000*(millis()/1000.);
//  motor1->setDesiredPosition(target_pos);
  motor1->setDesiredCurrent(0.1);
  
  Serial.print("M1: ");
//  Serial.print(motor1->getPosition());
  Serial.print(motor1->getCurrent());
  
  if (state == 0) {
    digitalWrite(led_pin, HIGH);
    motor1->drive(10);
  } else {
    digitalWrite(led_pin, LOW);
    motor1->drive(0);
  }
//  Serial.print("  M2: ");
//  Serial.print(motor2->getPosition());
//  Serial.print(", ");
//  Serial.print(motor1->calculateVelocity());
//  if (millis() < 1500) {
//    motor1->update();
//  } else {
//    motor1->drive(0);
//  }
  Serial.println();
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
