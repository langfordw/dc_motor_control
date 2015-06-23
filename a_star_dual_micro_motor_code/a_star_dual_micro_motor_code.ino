#include "dc_motor.h"
#include "stapler.h"

// BE SURE TO SET THE CORRECT INTERRUPT CORRESPONDING TO THE DIGITAL PIN OF THE ENCODER
#define encA1_int 0
#define encB1_int 1
#define encA2_int 2
#define encB2_int 3

// 11 and 12 are dummy pins that aren't actually used
DCMotor *motor1 = new DCMotor(5, 11, 10, A1, 3, 2, 10); //dir_pin1, dir_pin2, pwm_pin, current_sense_pin, encA_pin, encB_pin, counts_per_revolution
DCMotor *motor2 = new DCMotor(4, 12, 9, A0, 1, 0, 10);
// CW = NEGATIVE
// Drive values < 0 --> piston moves down, encoder decrements
// Drive values > 0 --> piston moves up, encoder increments

Stapler *stapler1 = new Stapler();

byte state = 0;
const byte startup = 1;
const byte up = 2;
const byte down = 3;
const byte off = 0;

byte activate_pin1 = 7;
byte activate_pin2 = 8;
byte led_pin = 13;

long tstart = 0;

int pos = 0;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  motor1->init();
  motor1->setCurrentLimit(0.1); // amps
  motor1->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor1->setPolarity(0);
  
  motor2->init();
  motor2->setCurrentLimit(0.1); // amps
  motor2->setPWMLimit(255); // given nominal 6v motor with 6V power supply
  motor2->setPolarity(1);

  motor1->setPIDGains(20.0,1.0,0); // current control
  motor1->setDesiredCurrent(-0.6);
  motor1->setControlType(0);
  tstart = millis();
  state = startup;
}

void loop() {
  if (Serial.available() > 0) {
    byte inByte = Serial.read();
    if (inByte == 'q') {
      motor1->setPIDGains(2.0,0.03,0); // position control
      motor1->setDesiredPosition(100);
      motor1->setControlType(1);
      tstart = millis();
      state = up;
      Serial.print("UP");
      Serial.println(tstart);
    } else if (inByte == 'a') {
      motor1->setPIDGains(2.0,0.03,0); // position control
      motor1->setDesiredPosition(0);
      motor1->setControlType(1);
      tstart = millis();
      state = down;
      Serial.print("DOWN");
    } else if (inByte == '1') {
      motor1->setPIDGains(20.0,1.0,0); // current control
      motor1->setDesiredCurrent(-0.7);
      motor1->setControlType(0);
      tstart = millis();
      state = startup;
      Serial.print("STARTUP");
    } else if (inByte == 'z') {
      state = off;
      Serial.print("OFF");
    }
    
    if (inByte == 'w') {
      motor2->setPIDGains(2.0,0.03,0); // position control
      motor2->setDesiredPosition(100);
      motor2->setControlType(1);
      tstart = millis();
      state = up;
      Serial.print("UP");
      Serial.println(tstart);
    } else if (inByte == 's') {
      motor1->setPIDGains(2.0,0.03,0); // position control
      motor1->setDesiredPosition(0);
      motor1->setControlType(1);
      tstart = millis();
      state = down;
      Serial.print("DOWN");
    } else if (inByte == '2') {
      motor1->setPIDGains(20.0,1.0,0); // current control
      motor1->setDesiredCurrent(-0.7);
      motor1->setControlType(0);
      tstart = millis();
      state = startup;
      Serial.print("STARTUP");
    } else if (inByte == 'x') {
      state = off;
      Serial.print("OFF");
    }
  }
  
  switch (state) {
    case startup:
      motor1->update();
      Serial.println();
      if ((millis() - tstart) > 1000) {
        motor1->setZero(); 
        state = off; 
      }
    break;
    case up:
      moveFromAtoB(motor1->getPosition(),2000,tstart,tstart+2000);
      motor1->update();
      Serial.println();
      if ((millis() - tstart) > 2000) { state = off; }
    break;
    case down:
      moveFromAtoB(motor1->getPosition(),0,tstart,tstart+2000);
      motor1->update();
      Serial.println();
      if ((millis() - tstart) > 2000) { state = off; }
    break;
    case off:
      motor1->drive(0);
      motor2->drive(0);
    break;
  }
  
  
//  
//  Serial.print("M1: ");
//  Serial.print(motor1->getPosition());
//  Serial.print(", ");
//  Serial.print(motor1->getCurrent());
//  
//  if (state == 0) {
//    digitalWrite(led_pin, HIGH);
////    motor1->drive(-100);
//      motor1->update();
//  } else {
//    digitalWrite(led_pin, LOW);
//    motor1->drive(0);
//  }
//  Serial.print("  M2: ");
//  Serial.print(motor2->getPosition());
//  Serial.print(", ");
//  Serial.print(motor1->calculateVelocity());
//  if (millis() < 1500) {
//    motor1->update();
//  } else {
//    motor1->drive(0);
//  }
//    Serial.println();
}

void moveFromAtoB(long a, long b, long tstart, long tend) {
  float alpha = float((millis()-tstart))/float((tend-tstart)); // 0 --> 1
  int pos = alpha*b + (1-alpha)*a; //a --> b
  motor1->setDesiredPosition(pos);
  Serial.println(alpha);
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
