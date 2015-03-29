/*
 *  low_pass_filter.h
 *  
 *
 *  Created by William Langford on 3/29/11.
 *  Copyright 2011 Tufts Robotics Club. All rights reserved.
 *
 */
#ifndef dc_motor_h
#define dc_motor_h

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include <avr/io.h>
 #include "WProgram.h"
#endif

#include "low_pass_filter.h"

class DCMotor
{
  public:
    DCMotor(byte dir_pin1, byte dir_pin2, byte pwm_pin, byte current_sense_pin, byte encA_pin, byte encB_pin, int counts_per_revolution);
    void init();
    void setCurrentLimit(float current_limit);
    void setPWMLimit(int max_pwm);
    
    float calculateVelocity();
    void update();
    void interruptRoutineA();
    void interruptRoutineB();
  private:
    byte _dir_pin1, _dir_pin2, _pwm_pin, _current_sense_pin, _encA_pin, _encB_pin;
    float _measureCurrent();
    volatile long _position;
    long _desired_position;
    float _velocity, _desired_velocity;
    float _current, _current_limit, _current_sensitivity;
    float _f_cs_smoothing;
    float _desired_current;
    unsigned volatile long _t_enc_triggered;
    int _max_pwm;
    long _delta_T, _last_t_enc_triggered;
    int  _counts_per_revolution;
    LPFilter *_currentFilter;
};

#endif
