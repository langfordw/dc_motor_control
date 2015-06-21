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
    void drive(int power);
    float calculateVelocity();
    void update();
    void interruptRoutineA();
    void interruptRoutineB();
    void setPolarity(int8_t polarity);
    void setPIDGains(float p, float i, float d);
    void setDesiredVelocity(float desired_velocity);
    void setDesiredPosition(long desired_position);
    void setDesiredForce(int desired_force);
    long getPosition();
    int measureForce();
  private:
    byte _dir_pin1, _dir_pin2, _pwm_pin, _current_sense_pin, _encA_pin, _encB_pin, _force_sense_pin;
    float _measureCurrent();
    volatile long _position;
    long _desired_position;
    int _desired_force;
    float _velocity, _desired_velocity;
    float _current, _current_limit, _current_sensitivity;
    void _velocityControl(), _positionControl(), _forceControl();
    float _desired_current;
    unsigned volatile long _t_enc_triggered;
    int _max_pwm;
    long _delta_T, _last_t_enc_triggered;
    int  _counts_per_revolution;
    LPFilter *_currentFilter, *_pwmFilter, *_velocityFilter;
    float _f_cs_smoothing, _f_pwm_smoothing, _f_vel_smoothing;
    int8_t _sgn(int val);
    int8_t _polarity;
    float _k_p, _k_i, _k_d;
    float _error, _sum_error, _last_error, _delta_error;
    volatile boolean _enc_trig_flag;
    int _last_trig;
    int8_t _dir;
    long _last_time, _time;
    long _last_position, _delta_pos;
};

#endif
