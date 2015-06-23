// DCMotor class
// derived from Amanda Ghassaei's DCMotorCode project:
// https://github.com/amandaghassaei/DCMotorCode

#include "DC_motor.h"
#include <avr/io.h>

DCMotor::DCMotor(byte dir_pin1, byte dir_pin2, byte pwm_pin, byte current_sense_pin, byte encA_pin, byte encB_pin, int counts_per_revolution)
{
  _dir_pin1 = dir_pin1;
  _dir_pin2 = dir_pin2;
  _pwm_pin = pwm_pin;
  _current_sense_pin = current_sense_pin;
  _encA_pin = encA_pin;
  _encB_pin = encB_pin;
  _enc_trig_flag = false;
  _force_sense_pin = A7;
  
  _position = 0;
  _desired_position = _position;
  
  _desired_force = 0;
  
  _velocity = 0;
  _dir = 1;
  _desired_velocity = _velocity;
  _delta_T = 0;
  _counts_per_revolution = counts_per_revolution;
  
  _current = 0;
  _current_sensitivity = 0.009775; //A / ADC Val  //140mV/A
  _desired_current = int(0.5/_current_sensitivity);
  
  _currentFilter = new LPFilter();
  _f_cs_smoothing = 2.5; //Hz
  
  _pwmFilter = new LPFilter();
  _f_pwm_smoothing = 10000; //Hz
  
  _velocityFilter = new LPFilter();
  _f_vel_smoothing = 10; //Hz
  
  _polarity = 0;
  _enc_polarity = 1;
  
  // velocity control gains
//  _k_p = 1.2;
//  _k_i = 0.8;
//  _k_d = 0;
  
  //position control gains
//  _k_p = 5.;
//  _k_i = 0.5;
//  _k_d = 0;
  
//  _k_p = 0.;
//  _k_i = 0.0;
//  _k_d = 500.;
  
  _k_p = 0.0;
  _k_i = 0.0;
  _k_d = 0.0;
}

void DCMotor::init()
{
  pinMode(_dir_pin1, OUTPUT);
  pinMode(_dir_pin2, OUTPUT);
  pinMode(_pwm_pin, OUTPUT);
  pinMode(_current_sense_pin, INPUT);
  pinMode(_encA_pin, INPUT);
  pinMode(_encB_pin, INPUT);   
  
  _t_enc_triggered = micros();
  _last_t_enc_triggered = _t_enc_triggered;
  
  _currentFilter->init(_f_cs_smoothing, this->_measureCurrent());
  _pwmFilter->init(_f_pwm_smoothing, 0);
  _velocityFilter->init(_f_vel_smoothing, 0);
}

void DCMotor::setCurrentLimit(float current_limit)
{
  _current_limit = current_limit/_current_sensitivity;
}

void DCMotor::setPWMLimit(int max_pwm)
{
  _max_pwm = max_pwm;
}

void DCMotor::setPIDGains(float p, float i, float d) 
{
  _k_p = p;
  _k_i = i;
  _k_d = d;
}

float DCMotor::_measureCurrent() 
{
  return _currentFilter->step(analogRead(_current_sense_pin)); 
}

long DCMotor::getPosition()
{
  return _position;
}

float DCMotor::getCurrent()
{
  return _measureCurrent();
}

int DCMotor::measureForce()
{
  return analogRead(_force_sense_pin);
}

void DCMotor::setControlType(byte control_type)
{
  // 0 = current
  // 1 = position
  _control_type = control_type;
}

void DCMotor::update()
{
  switch (_control_type) {
    case 0:
      this->_currentControl();
    break;
    case 1:
      this->_positionControl();
    break;
  }
}

void DCMotor::updateCurrentControl()
{
//  this->_velocityControl();
//  this->_positionControl();
//  this->_forceControl();
  this->_currentControl();
}

void DCMotor::updatePositionControl()
{
//  this->_velocityControl();
  this->_positionControl();
//  this->_forceControl();
//  this->_currentControl();
}

void DCMotor::_velocityControl()
{
  calculateVelocity();
  _last_error = _error;
  _error = _desired_velocity-_velocity;
  _delta_error = _error-_last_error;
  _sum_error += _error;
  _sum_error = constrain(_sum_error, -100, 100);
  float pwr = _pwmFilter->step(_k_p * _error + _k_i * _sum_error + _k_d * _delta_error);
  if (_desired_velocity >= 0) { pwr = constrain(pwr,0,255); }
  else { pwr = constrain(pwr,-255,0); }
  Serial.print(", ");
  Serial.print(pwr);
  drive(int(pwr));
}

void DCMotor::_positionControl()
{
  _last_error = _error;
  _error = _desired_position-_position;
  _delta_error = _error-_last_error;
  _sum_error += _error;
  _sum_error = constrain(_sum_error, -1000, 1000);
  int pwr = _pwmFilter->step(int(_k_p * _error + _k_i * _sum_error + _k_d * _delta_error));
  Serial.print(_position);
  Serial.print(", ");
  Serial.print(pwr);
  drive(pwr);
}

void DCMotor::_forceControl()
{
  _last_error = _error;
  _error = _desired_force-measureForce();
  _delta_error = _error-_last_error;
  _sum_error += _error;
  _sum_error = constrain(_sum_error, -100, 100);
  int pwr = _pwmFilter->step(int(_k_p * _error + _k_i * _sum_error + _k_d * _delta_error));
  Serial.print(", ");
  Serial.print(_error);
  Serial.print(", ");
  Serial.print(pwr);
  drive(pwr);
}

void DCMotor::_currentControl()
{
  if (_desired_current > 0) {
    _current = _measureCurrent();
  } else {
    _current = -_measureCurrent();
  }
  _error = _desired_current - _current;
  _sum_error += _error;
  _sum_error = constrain(_sum_error, -100, 100);
  int pwr = int(_k_p * _error);
  Serial.print(_position);
  Serial.print(", ");
  Serial.print(pwr);
  drive(pwr);
}

void DCMotor::setDesiredVelocity(float desired_velocity) 
{
  _desired_velocity = desired_velocity;
}

void DCMotor::setDesiredPosition(long desired_position) 
{
  _desired_position = desired_position;
}

void DCMotor::setDesiredForce(int desired_force) 
{
  _desired_force = desired_force;
}

void DCMotor::setDesiredCurrent(float desired_current)
{
  _desired_current = int(desired_current/_current_sensitivity);
}

void DCMotor::setPolarity(int8_t polarity)
{
  _polarity = polarity;
}

void DCMotor::setEncPolarity(int8_t enc_polarity)
{
  _enc_polarity = enc_polarity;
}

void DCMotor::setZero()
{
  _position = 0;
}

void DCMotor::drive(int power)
{
  if (_sgn(power) < 0) {
    if (!_polarity) {
      digitalWrite(_dir_pin1, HIGH);
      digitalWrite(_dir_pin2, LOW);
    } else {
      digitalWrite(_dir_pin1, LOW);
      digitalWrite(_dir_pin2, HIGH);
    }
  } else if (_sgn(power) > 0) {
    if (!_polarity) {
      digitalWrite(_dir_pin1, LOW);
      digitalWrite(_dir_pin2, HIGH);
    } else {
      digitalWrite(_dir_pin1, HIGH);
      digitalWrite(_dir_pin2, LOW);
    }
  } else {
    // if power = 0, coast
    digitalWrite(_dir_pin1, LOW);
    digitalWrite(_dir_pin2, LOW);
  }
  uint8_t pwr = constrain(abs(power),0,_max_pwm);
  analogWrite(_pwm_pin, pwr);
}

float DCMotor::calculateVelocity()
{
  // TO DO: add timeout for velocity = 0?
//  if (_enc_trig_flag) {
//    // encoder triggered
//    _delta_T = _t_enc_triggered-_last_t_enc_triggered;
//    if (_delta_T > 0) _velocity = 1000000/_delta_T; // counts/sec
//    _enc_trig_flag = false;
//    _last_trig = millis();
//  } else {
//    // no encoder tick, are we stopped?
//    if (millis()-_last_trig > 100) { // 100 --> mininum measurable speed = 10 counts/sec
//      _velocity = 0;
//    }
//  }

  // DELTA_POS METHOD
  _time = micros();
  _delta_T = _time-_last_time;
  _last_time = _time;
  
  _delta_pos = _position-_last_position;
  _last_position = _position;
  
  _velocity = _velocityFilter->step(_delta_pos*1000000/_delta_T);

  // DELTA_T METHOD
//  _delta_T = _t_enc_triggered-_last_t_enc_triggered;
//  if (_delta_T > 0) _velocity = 1000000/_delta_T; // counts/sec
//  _velocity = constrain(_velocity,0,3200); // determined experimentally by spinnning motor at full power
  // max no load velocity = 3000.00 @ 6V, 255 pwm (about .145A current drawn from power supply)
  // = 5800 rpm
  // 32 counts/rev
//  if (_dir < 0) _velocity*=_dir;

  return _velocity;
}

void DCMotor::interruptRoutineA()
{
//  if (PIND & (1 << PORTD0)) {
//    if (!(PIND & (1 << PORTD1))) { 
//      _position++;
//     _dir = 1; 
//    } 
//    else { 
//      _position--; 
//      _dir = -1;
//    }
//  } else {
//    if (!(PIND & (1 << PORTD1))) { 
//      _position--; 
//      _dir = -1;
//    } 
//    else { 
//      _position++; 
//      _dir = 1;
//    }
//  }
  if (_enc_polarity) {
    if (digitalRead(_encA_pin)) {
      if (!digitalRead(_encB_pin)) { 
        _position++;
       _dir = 1; 
      } 
      else { 
        _position--; 
        _dir = -1;
      }
    } else {
      if (!digitalRead(_encB_pin)) { 
        _position--; 
        _dir = -1;
      } 
      else { 
        _position++; 
        _dir = 1;
      }
    }
  } else {
    if (digitalRead(_encA_pin)) {
      if (!digitalRead(_encB_pin)) { 
        _position--;
       _dir = 1; 
      } 
      else { 
        _position++; 
        _dir = -1;
      }
    } else {
      if (!digitalRead(_encB_pin)) { 
        _position++; 
        _dir = -1;
      } 
      else { 
        _position--; 
        _dir = 1;
      }
    }
  }
  
  _last_t_enc_triggered = _t_enc_triggered;
  _t_enc_triggered = micros();
  _enc_trig_flag = true;
}

void DCMotor::interruptRoutineB()
{
  // todo: figure out gray code for properly incrementing position after being triggered by encB
  _last_t_enc_triggered = _t_enc_triggered;
  _t_enc_triggered = micros();
  _enc_trig_flag = true;
}

int8_t DCMotor::_sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}
