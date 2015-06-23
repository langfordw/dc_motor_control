#include "stapler.h"
#include <avr/io.h>

Stapler::Stapler(DCMotor **motor)
{
  _motor = *motor;
  
  _off = 0;
  _up = 1;
  _down = 2;
  _startup = 3;
  
  _state = _off;
  
}

void Stapler::startup()
{
  _motor->setPIDGains(20.0,1.0,0); // current control
  _motor->setDesiredCurrent(-0.7);
  _motor->setControlType(0);
  _tstart = millis();
  _state = _startup;
  Serial.println("STARTUP");
}

void Stapler::up()
{
  _motor->setPIDGains(2.0,0.03,0); // position control
  _motor->setControlType(1);
  _tstart = millis();
  _state = _up;
  Serial.println("UP");
}

void Stapler::down()
{
  _motor->setPIDGains(2.0,0.03,0); // position control
  _motor->setControlType(1);
  _tstart = millis();
  _state = _down;
  Serial.println("DOWN");
}

void Stapler::off()
{
  _state = _off;
  Serial.println("OFF");
}

void Stapler::moveFromAtoB(long a, long b, long tstart, long tend) {
  float alpha = float((millis()-tstart))/float((tend-tstart)); // 0 --> 1
  int pos = alpha*b + (1-alpha)*a; //a --> b
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
          _state = _off;
          Serial.println("OFF");
        }
    }
    else if (_state == _up) {
       this->moveFromAtoB(_motor->getPosition(),2000,_tstart,_tstart+2000);
       if ((millis() - _tstart) > 2000) {
          _state = _off;
          Serial.println("OFF");
        }
    }
    else if (_state == _down) {
       this->moveFromAtoB(_motor->getPosition(),0,_tstart,_tstart+2000);
       if ((millis() - _tstart) > 2000) {
          _state = _off;
          Serial.println("OFF");
        }
    }
  }
}
  

//void Stapler::attachMotor(DCMotor *motor)
//{
//  
//}
