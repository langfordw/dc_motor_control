// DCMotor class
// derived from Amanda Ghassaei's DCMotorCode project:
// https://github.com/amandaghassaei/DCMotorCode

#include "DC_motor.h"

DCMotor::DCMotor(byte dir_pin1, byte dir_pin2, byte pwm_pin, byte current_sense_pin, byte encA_pin, byte encB_pin, int counts_per_revolution)
{
  _dir_pin1 = dir_pin1;
  _dir_pin2 = dir_pin2;
  _current_sense_pin = current_sense_pin;
  _encA_pin = encA_pin;
  _encB_pin = encB_pin;
  
  _position = 0;
  _desired_position = _position;
  
  _velocity = 0;
  _desired_velocity = _velocity;
  _delta_T = 0;
  _counts_per_revolution = counts_per_revolution;
  
  _current = 0;
  _current_sensitivity = 0.03491; //A / ADC Val  //140mV/A
  _desired_current = int(0.8/_current_sensitivity);
  
  _currentFilter = new LPFilter();
  _f_cs_smoothing = 2.5; //Hz
  
}

void DCMotor::init()
{
  pinMode(_dir_pin1, OUTPUT);
  pinMode(_dir_pin2, OUTPUT);
  pinMode(_current_sense_pin, INPUT);
  pinMode(_encA_pin, INPUT);
  pinMode(_encB_pin, INPUT);   
  
  _t_enc_triggered = micros();
  _last_t_enc_triggered = _t_enc_triggered;
  
  _currentFilter->init(_f_cs_smoothing, this->_measureCurrent());
}

void DCMotor::setCurrentLimit(float current_limit)
{
  _current_limit = current_limit;
}

void DCMotor::setPWMLimit(int max_pwm)
{
  _max_pwm = max_pwm;
}

float DCMotor::_measureCurrent() 
{
  return analogRead(_current_sense_pin)*_current_sensitivity; 
}

void DCMotor::update()
{
  
}

float DCMotor::calculateVelocity()
{
  _delta_T = _t_enc_triggered-_last_t_enc_triggered;
  _velocity = 60000000/_delta_T/_counts_per_revolution; // rev/s
  return _velocity;
}

void DCMotor::interruptRoutineA()
{
  if (digitalRead(_encA_pin)) {
    if (!digitalRead(_encB_pin)) { _position++; } 
    else { _position--; }
  } else {
    if (!digitalRead(_encB_pin)) { _position--; } 
    else { _position++; }
  }
  
  _t_enc_triggered = micros();
}

void DCMotor::interruptRoutineB()
{
  // todo: figure out gray code for properly incrementing position after being triggered by encB
  _t_enc_triggered = micros();
}
