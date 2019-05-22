/*
	MLX Hall Sensor reading
	Reads From both sensors at addresses 0x0D (right) and 0x0C (left)
*/

#ifndef MLX_h
#define MLX_h

#include "Arduino.h"

class MLX {
	public:
		MLX(int addr);
		void begin();
    void begin(bool init);
		void beginRight();
		void beginLeft();
		void process();
		void processRight();
		void processLeft();
		int getThrottle();
		int getYaw();
		int getPitch();
		int getRoll();
	
	private:  //Put here variables (in pin for example)
    int _ThrottleReading;
    int _YawReading; 
    int _PitchReading;
    int _RollReading;
    int LeftAddr;	
};

#endif
