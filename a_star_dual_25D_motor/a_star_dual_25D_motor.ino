#include <low_pass_filter.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

#define encA1 1
#define encB1 0
#define encA2 3
#define encB2 2
#define encA1_int 3
#define encB1_int 2
#define encA2_int 0
#define encB2_int 1

LPFilter lpfilter1, lpfilter2;

volatile long motor_pos1, motor_pos2 = 0;
unsigned volatile long last_trig1, t_trig1, dt1;

uint8_t led_pin = 13;
uint8_t inA1 = 5;
uint8_t inB1 = 4;
uint8_t inA2 = 6;
uint8_t inB2 = 7;
uint8_t pwm1 = 10;
uint8_t pwm2 = 9;
uint8_t c_sense1 = A1;
uint8_t c_sense2 = A0;

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

boolean off = true;

void setup() {
  Serial.begin(9600);
  Serial.println("started...");
  
  pinMode(led_pin, OUTPUT); 
  pinMode(inA1, OUTPUT);
  pinMode(inB1, OUTPUT);
  pinMode(inA2, OUTPUT);
  pinMode(inB2, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);

  pinMode(encA1, INPUT);
  pinMode(encB1, INPUT);
  pinMode(encA2, INPUT);
  pinMode(encB2, INPUT);
  pinMode(c_sense1, INPUT);
  pinMode(c_sense2, INPUT);
  
  attachInterrupt(encA1_int, encA1trig, CHANGE);
  attachInterrupt(encA2_int, encA2trig, CHANGE);
  
  lpfilter1.init(2.5, measureCurrentInt1());
  lpfilter2.init(2.5, measureCurrentInt2());
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
  
  driveMotor(1,constrain(motor_pwr2,0,255));

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

void driveMotor(uint8_t which_motor, float pwr) {
  uint8_t m_pwm = constrain(abs(pwr),0,max_pwm);
  int dir = sgn(int(pwr));
  switch(which_motor) {
    case 1:
      if (dir > 0) {
        digitalWrite(inA1, HIGH);
        digitalWrite(inB1, LOW);
      } else if (dir < 0) {
        digitalWrite(inA1, LOW);
        digitalWrite(inB1, HIGH);
      } else {
        digitalWrite(inA1, HIGH);
        digitalWrite(inB1, HIGH);
      }
      analogWrite(pwm1, m_pwm);
    break;
    case 2:
      if (dir > 0) {
        digitalWrite(inA2, HIGH);
        digitalWrite(inB2, LOW);
      } else if (dir < 0) {
        digitalWrite(inA2, LOW);
        digitalWrite(inB2, HIGH);
      } else {
        digitalWrite(inA2, HIGH);
        digitalWrite(inB2, HIGH);
      }
      analogWrite(pwm2, m_pwm);
    break;
  }
}

static inline int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}

float measureCurrent1() {
  // 140mV/A
  return (analogRead(c_sense1)*0.03491); //adc to A
}

uint16_t measureCurrentInt1() {
  return analogRead(c_sense1);
}


float measureCurrent2() {
  // 140mV/A
  return (analogRead(c_sense2)*0.03491); //adc to A
}

uint16_t measureCurrentInt2() {
  return analogRead(c_sense2);
}

void encA1trig() {
  if (digitalRead(encA1) == HIGH) {
    if (digitalRead(encB1) == LOW) {
      motor_pos1++;
    } else {
      motor_pos1--;
    }
  } else {
    if (digitalRead(encB1) == LOW) {
      motor_pos1--;
    } else {
      motor_pos1++;
    }
  }
  
//  if (dt1 > 1000) { dt1 = 1000; }
}

void encA2trig() {
  if (digitalRead(encA2) == HIGH) {
    if (digitalRead(encB2) == LOW) {
      motor_pos2++;
    } else {
      motor_pos2--;
    }
  } else {
    if (digitalRead(encB2) == LOW) {
      motor_pos2--;
    } else {
      motor_pos2++;
    }
  }
  last_trig1 = t_trig1;
  t_trig1 = micros();
  dt1 = t_trig1 - last_trig1;
}
