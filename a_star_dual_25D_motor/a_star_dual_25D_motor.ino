#include "dc_motor.h"
#include "low_pass_filter.h"

#define encA1_int 3
#define encB1_int 2
#define encA2_int 0
#define encB2_int 1

LPFilter lpfilter1, lpfilter2;

volatile long motor_pos1, motor_pos2 = 0;
unsigned volatile long last_trig1, t_trig1, dt1;

int activate_state, last_activate_state = 0;

uint16_t setCurrent1 = uint16_t(0.8/0.009775); //0.8 Amps
uint16_t setCurrent2 = uint16_t(0.8/0.009775); //0.8 Amps
float thisCurrent1 = 0, thisCurrent2 = 0;
float motor_pwr1 = 0, motor_pwr2 = 0;
int error1 = 0, error2 = 0;
float kp1 = 20.8, kp2 = 20.8;
float ki1= 2.3, ki2 = 2.3; //2.3
uint8_t max_pwm = 100;
int sum_error;

int last_t, t, dt;
int dpos1, last_motor_pos1;
float vel1, vel2;

uint16_t t_start = 0;

DCMotor *motor1 = new DCMotor(5, 4, 10, A1, 1, 0, 1000);
DCMotor *motor2 = new DCMotor(6, 7, 9, A0, 3, 2, 1000);

boolean off = true;

void setup() {
  Serial.begin(9600);
  Serial.println("started...");
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  motor1->init();
  motor2->init();
}

void loop() {
//  //DEBUG
//  analogWrite(pwm1, 50);
//  digitalWrite(dir1, LOW);
//  analogWrite(pwm2, 50);
//  digitalWrite(dir2, LOW);
//  driveMotor(1,-50);
  
//  digitalWrite(inA1, LOW);
//  digitalWrite(inB1, HIGH);
//  analogWrite(pwm1,50);

//  t = millis();
//  dt = t - last_t;
//  last_t = t;
//  
  dpos1 = last_motor_pos1 - motor_pos1;
  last_motor_pos1 = motor_pos1;
//  
//  vel1 = dpos1/dt;

  if (dt1 > 5000) { dt1 = 5000; }
  int error = dt1-500;
  sum_error += error;
  sum_error = constrain(sum_error,-10000,10000);
  motor_pwr2 = 0.05*int(dt1-1000) + 0.01*sum_error;
  

  // Serial output for debugging
  Serial.print("M1: ");
  Serial.print(motor_pos2);
  Serial.print(", ");
  Serial.print(motor_pwr2);
  Serial.print(", ");
  Serial.println(dt1);
//  Serial.print("   M2: ");
//  Serial.print(motor_pos2);
//  Serial.print(", ");
//  Serial.print(thisCurrent2);
//  Serial.println();
  
//  // Read serial in for debugging
//  if (Serial.available()) {
//    char input = Serial.read();
//    if (input == 'u') {
//      digitalWrite(led_pin, LOW);
//      digitalWrite(dir1, HIGH);
//      digitalWrite(dir2, HIGH);
//      Serial.println("up");
//      error1 = 0;
//      error2 = 0;
//      t_start = millis();
//    } else if (input == 'd') {
//      digitalWrite(led_pin, HIGH);
//      digitalWrite(dir1, LOW);
//      digitalWrite(dir2, LOW);
//      Serial.println("down");
//      t_start = millis();
//      error1 = 0;
//      error2 = 0;
//    } else {
//    }
//  }
//
//  //Check activate pin
//  activate_state = digitalRead(activate_pin);
//  if (activate_state != last_activate_state) {
//    //Debounce
//    delay(10);
//    activate_state = digitalRead(activate_pin);
//    //if it's changed...
//    if (activate_state != last_activate_state) {
//      if (activate_state) {
//        digitalWrite(led_pin, HIGH);
//        digitalWrite(dir1, LOW);
//        digitalWrite(dir2, LOW);
//        Serial.println("down");
//      } else {
//        digitalWrite(led_pin, LOW);
//        digitalWrite(dir1, HIGH);
//        digitalWrite(dir2, HIGH);
//        Serial.println("up");
//      }
//      last_activate_state = activate_state;
//      t_start = millis();
//    }
//  } 
//  
//  // Main current control update
//  thisCurrent1 = lpfilter1.step(measureCurrentInt1());
//  error1 += setCurrent1-thisCurrent1;
//  error1 = constrain(error1,-100,100);
//  motor_pwr1 = constrain(kp1*(setCurrent1-thisCurrent1)+ki1*error1,0,max_pwm);
//  
//  thisCurrent2 = lpfilter2.step(measureCurrentInt2());
//  error2 += setCurrent2-thisCurrent2;
//  error2 = constrain(error2,-100,100);
//  motor_pwr2 = constrain(kp2*(setCurrent2-thisCurrent2)+ki2*error2,0,max_pwm);
//  
//  // stop after 1 second
//  uint16_t dt = millis()-t_start;
//  if (dt < 1000) {
//    analogWrite(pwm1, motor_pwr1);
//    analogWrite(pwm2, motor_pwr2);
//  } else {
//    analogWrite(pwm1, 0);
//    analogWrite(pwm2, 0);
//  }
}

void encA1trig() {
  motor1->interruptRoutineA();
}

void encA2trig() {
  motor2->interruptRoutineA();
}
