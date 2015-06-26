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

Stapler *stapler1 = new Stapler(&motor1);
Stapler *stapler2 = new Stapler(&motor2);

byte state = 0;
const byte startup = 1;
const byte up = 2;
const byte down = 3;
const byte off = 0;

const byte activate_pin1 = 7;
const byte activate_pin2 = 8;
const byte led_pin = 13;

boolean which_stapler;
boolean trig_stapler = 0, last_trig_stapler;

long tstart = 0;

int pos = 0;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  pinMode(activate_pin1, INPUT);
  pinMode(activate_pin2, INPUT);
  
  motor1->init();
  motor1->setCurrentLimit(0.1); // amps
  motor1->setPWMLimit(150); // given nominal 6v motor with 6V power supply
  motor1->setPolarity(0);
  
  motor2->init();
  motor2->setCurrentLimit(0.1); // amps
  motor2->setPWMLimit(150); // given nominal 6v motor with 6V power supply
  motor2->setPolarity(0);
  motor2->setEncPolarity(0);
  
  stapler1->startup();
  stapler2->startup();
}

void loop() {
  
//  TO DO:
//    trigger from digital input pins
//    auto-switch states (rather than timeout)
//    
  
  if (Serial.available() > 0) {
    byte inByte = Serial.read();
    if (inByte == 'q') {
      stapler1->up();
    } else if (inByte == 'a') {
      stapler1->down();
    } else if (inByte == '1') {
      stapler1->startup();
    } else if (inByte == 'z') {
      stapler1->off();
    }
    
    if (inByte == 'w') {
      stapler2->up();
    } else if (inByte == 's') {
      stapler2->down();
    } else if (inByte == '2') {
      stapler2->startup();
    } else if (inByte == 'x') {
      stapler2->off();
    }
  }
  
  which_stapler = digitalRead(activate_pin2);
  last_trig_stapler = trig_stapler;
  trig_stapler = digitalRead(activate_pin1);
  
  if (trig_stapler != last_trig_stapler && trig_stapler) {
    Serial.println("triggered!");
    if (which_stapler) {
      stapler1->down();
      Serial.println("stapler 1");
    } else {
      stapler2->down();
      Serial.println("stapler 2");
    }
  }
  
  stapler1->update();
  stapler2->update();
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
