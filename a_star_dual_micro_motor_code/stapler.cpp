#include "stapler.h"
#include <avr/io.h>

Stapler::Stapler(DCMotor **motor, int encoder_counts_per_stroke)
{
  _motor = *motor;
  
  _off = 0;
  _up = 1;
  _down = 2;
  _startup = 3;
  _push = 4;
  
  _dt_up = 750;
  _dt_down = 750;
  
  _encoder_counts_per_stroke = encoder_counts_per_stroke;
  
  _state = _off;
}

void Stapler::startup()
{
  _motor->setPIDGains(20.0,1.0,0); // current control
  _motor->setDesiredCurrent(-0.7);
  _motor->setControlType(0);
  _tstart = millis();
  _start_pos = 0;
  _state = _startup;
  Serial.println("STARTUP");
}

void Stapler::push()
{
  _motor->setPIDGains(20.0,1.0,0); // current control
  _motor->setDesiredCurrent(-0.7);
  _motor->setControlType(0);
  _tstart = millis();
  _start_pos = _motor->getPosition();
  _state = _push;
  Serial.println("PUSH");
}

void Stapler::up()
{
  _motor->setPIDGains(1.5,0.01,0); // position control
  _motor->setControlType(1);
  _tstart = millis();
  _start_pos = _motor->getPosition();
  _state = _up;
  Serial.println("UP");
}

void Stapler::down()
{
  _motor->setPIDGains(2.0,0.03,0); // position control
  _motor->setControlType(1);
  _tstart = millis();
  _start_pos = _motor->getPosition();
  _state = _down;
  Serial.println("DOWN");
}

void Stapler::off()
{
  _state = _off;
  Serial.println("OFF");
}

//void Stapler::moveFromAtoB(int a, int b, long tstart, long tend) {
//  float alpha = float((millis()-tstart))/float((tend-tstart)); // 0 --> 1
//  int pos = alpha*b + (1-alpha)*a; //a --> b
//  _motor->setDesiredPosition(pos);
//  Serial.print(_tstart);
//  Serial.print(", ");
//  Serial.print(millis());
//  Serial.print(", ");
//  Serial.print(pos);
//  Serial.print(", ");
//  Serial.print(alpha);
//}

//void Stapler::moveFromAtoB(int a, int b, int tstart, int tend) {
//  float alpha = float((millis()-tstart))/float((tend-tstart)); // 0 --> 1
//  int pos = a+alpha*(b-a); //a --> b
//  _motor->setDesiredPosition(pos);
//  Serial.print(_tstart);
//  Serial.print(", ");
//  Serial.print(millis());
//  Serial.print(", ");
//  Serial.print((b-a));
//}

//void Stapler::moveFromAtoB(long a, long b, long tstart, long tend) {
//  float alpha = float((millis()-tstart))/float((tend-tstart))*1.571; // 0 --> pi/2
//  int dx = b-a;
//  int pos = a + float(0.5*(1.0-float(cos(2.0*alpha)))*float(dx)); //a --> b using sin^2(t)
//  Serial.print(_tstart);
//  Serial.print(", ");
//  Serial.print(millis());
//  Serial.print(", ");
//  Serial.print(pos);
//  _motor->setDesiredPosition(pos);
//}

void Stapler::moveFromAtoB(int a, int b, long tstart, long tend) {
  float alpha = float(millis()-tstart)/float(tend-tstart); // 0 --> 1
  int dx = b-a;
  int pos = a + float((3.0-2.0*alpha)*alpha*alpha)*dx; //a --> b using sin^2(t)
  Serial.print(_tstart);
  Serial.print(", ");
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(pos);
  _motor->setDesiredPosition(pos);
}

void Stapler::update()
{
  if (_state == _off) {
    _motor->drive(0);
  } else {
    _motor->update();
    Serial.println();
    
    if (_state == _startup) {
       if ((millis() - _tstart) > 1000) {
          _motor->setZero(); 
          Serial.println("set zero");
          this->off();
        }
    }
    else if (_state == _push) {
      if ((millis() - _tstart) > 250) {
          _motor->setZero(); 
          Serial.println("re-zero'd");
          this->up();
        }
    }
    else if (_state == _up) {
       this->moveFromAtoB(_start_pos,_encoder_counts_per_stroke,_tstart,_tstart+_dt_up);
       if ((millis() - _tstart) > _dt_up) {
          this->off();
        }
    }
    else if (_state == _down) {
       this->moveFromAtoB(_start_pos,50,_tstart,_tstart+_dt_down);
       if ((millis() - _tstart) > _dt_down) {
          this->push();
        }
    }
  }
}

int Stapler::getState()
{
  return _state;
}

//void Stapler::attachMotor(DCMotor *motor)
//{
//  
//}
