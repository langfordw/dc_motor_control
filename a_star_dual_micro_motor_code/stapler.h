#include "dc_motor.h"

#ifndef stapler_h
#define stapler_h

class Stapler
{
  public:
    Stapler(DCMotor **motor, int encoder_counts_per_stroke);
//    void attachMotor(DCMotor *motor);
    void startup();
    void up();
    void down();
    void off();
    void update();
    void moveFromAtoB(long a, long b, long tstart, long tend);
    int getState();
  private:
    DCMotor *_motor;
    byte _state, _off, _up, _down, _startup;
    long _tstart;
    int _encoder_counts_per_stroke;
};

#endif
