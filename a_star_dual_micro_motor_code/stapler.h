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
    void push();
    void update();
    void moveFromAtoB(int a, int b, long tstart, long tend);
    int getState();
  private:
    DCMotor *_motor;
    byte _state, _off, _up, _down, _startup, _push;
    long _tstart;
    int _encoder_counts_per_stroke;
    int _dt_down, _dt_up;
    int _start_pos;
    int _delPos;
};

#endif
