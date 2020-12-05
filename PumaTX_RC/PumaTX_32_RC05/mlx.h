/*
  MLX Hall Sensor reading
  Reads From both sensors at addresses 0x0D (right) and 0x0C (left)
*/

#ifndef MLX_h
#define MLX_h

#include "Arduino.h"

class MLX {
  public:
    MLX(int left_addr, int right_addr);
    void begin();
    void begin(bool init);
    void begin_right();
    void begin_left();
    void process();
    void process_right();
    void process_left();
    int get_throttle();
    int get_yaw();
    int get_pitch();
    int get_roll();
  
  private:  //Put here variables (in pin for example)
    int _throttle_reading;
    int _yaw_reading; 
    int _pitch_reading;
    int _roll_reading;
    int left_addr; 
    int right_addr;
};

#endif
