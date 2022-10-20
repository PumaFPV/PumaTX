/*
  MLX30393 Hall Sensor reading
  Reads From both sensors at addresses 0x0D (right) and 0x0C (left)
*/

#ifndef MLX_h
#define MLX_h

#include "Arduino.h"
#include <Wire.h>

class MLX {
  public:
    MLX(TwoWire *mlxI2C, int leftAddr, int rightAddr);
    void begin();
    void begin(bool init);
    void beginRight();
    void beginLeft();
    void process();
    void processRight();
    void processLeft();
    volatile int getThrottle();
    volatile int getYaw();
    volatile int getPitch();
    volatile int getRoll();
  
  private:  //Put here variables (in pin for example)
    TwoWire *_mlxI2C;
    int _throttleReading;
    int _yawReading; 
    int _pitchReading;
    int _rollReading;
    int leftAddr; 
    int rightAddr;
};

#endif
