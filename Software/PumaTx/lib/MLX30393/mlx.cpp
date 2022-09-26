/*
	MLX.cpp - Library for reading 2   hall sensors
	0x0C is Left gimbal
	0x0D is Right gimbal
	
	Ask for MLX.begin(); in void setup and MLX.process(); in loop
	Then get value from = MLX.get_throttle();
	
	Created by Nicolas de Grave, May 19th, 2019
*/

#include "Arduino.h"
#include "mlx.h"
#include <Wire.h>

int _throttle_reading;
int _yaw_reading;
int _pitch_reading;
int _roll_reading;
int _left_addr;
int _right_addr;
TwoWire _mlxI2C;

MLX::MLX(TwoWire mlxI2C, int left_addr, int right_addr)
{    
  _mlxI2C = mlxI2C; 
  _left_addr = left_addr;
  _right_addr = right_addr;
}


void MLX::begin() 
{
  MLX::begin(true);
}

//For void setup();
void MLX::begin(bool init)
{
  if (init) {    
  MLX::begin_left();
  MLX::begin_right(); 
  }
}


void MLX::begin_left() //0x0C
{	

	_mlxI2C.beginTransmission(_left_addr);   // Start I2C Transmission
  _mlxI2C.write(0x60);   // Select Write register command
  _mlxI2C.write(0x00);   // Set AH = 0x00, BIST disabled
  _mlxI2C.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  _mlxI2C.write(0x00);   // Select address register, (0x00 << 2)
  _mlxI2C.endTransmission();   // Stop I2C Transmission
  _mlxI2C.requestFrom(_left_addr, 1);    // Request 1 byte of data
 
  if(_mlxI2C.available() == 1){   // Read status byte
    unsigned int c = _mlxI2C.read();
  }
 
    _mlxI2C.beginTransmission(_left_addr);   // Start I2C Transmission
    _mlxI2C.write(0x60);   // Select Write register command
    _mlxI2C.write(0x02);   // Set AH = 0x02
    _mlxI2C.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    _mlxI2C.write(0x08);   // Select address register, (0x02 << 2)
    _mlxI2C.endTransmission();   // Stop I2C Transmission
    _mlxI2C.requestFrom(_left_addr, 1);    // Request 1 byte of data  

  if(_mlxI2C.available() == 1){   // Read status byte
    unsigned int c = _mlxI2C.read();
  }
}	
	
void MLX::begin_right(){	//0x0D

	_mlxI2C.beginTransmission(_right_addr);   // Start I2C Transmission
  _mlxI2C.write(0x60);   // Select Write register command
  _mlxI2C.write(0x00);   // Set AH = 0x00, BIST disabled
  _mlxI2C.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  _mlxI2C.write(0x00);   // Select address register, (0x00 << 2)
  _mlxI2C.endTransmission();   // Stop I2C Transmission
  _mlxI2C.requestFrom(_right_addr, 1);    // Request 1 byte of data
 
  if(_mlxI2C.available() == 1){   // Read status byte
    unsigned int c = _mlxI2C.read();
  }
 
    _mlxI2C.beginTransmission(_right_addr);   // Start I2C Transmission
    _mlxI2C.write(0x60);   // Select Write register command
    _mlxI2C.write(0x02);   // Set AH = 0x02
    _mlxI2C.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    _mlxI2C.write(0x08);   // Select address register, (0x02 << 2)
    _mlxI2C.endTransmission();   // Stop I2C Transmission
    _mlxI2C.requestFrom(_right_addr, 1);    // Request 1 byte of data  

  if(_mlxI2C.available() == 1){   // Read status byte
    unsigned int c = _mlxI2C.read();
  }
}


//For void loop();
void MLX::process(){
	
	MLX::process_left();
	MLX::process_right();
	
}

void MLX::process_left(){
  
  int throttle_reading;
  int yaw_reading;
	
  unsigned int data[7];
  
    _mlxI2C.beginTransmission(_left_addr);   // Start I2C Transmission
    _mlxI2C.write(0x3E);   // Start single meaurement mode, ZYX enabled
    _mlxI2C.endTransmission();   // Stop I2C Transmission
    _mlxI2C.requestFrom(_left_addr, 1);    // Request 1 byte of data
 
  if(_mlxI2C.available() == 1){   // Read status byte
    unsigned int c = _mlxI2C.read();
  }

    _mlxI2C.beginTransmission(_left_addr);   // Start I2C Transmission
    _mlxI2C.write(0x4E);   // Send read measurement command, ZYX enabled
    _mlxI2C.endTransmission();   // Stop I2C Transmission
    _mlxI2C.requestFrom(_left_addr, 7);    // Request 7 bytes of data
 
  if(_mlxI2C.available() == 7){;    // Read 7 bytes of data   status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb

    data[0] = _mlxI2C.read();
    data[1] = _mlxI2C.read();
    data[2] = _mlxI2C.read();
    data[3] = _mlxI2C.read();
    data[4] = _mlxI2C.read();
    data[5] = _mlxI2C.read();
    data[6] = _mlxI2C.read();
  }
 
  // Convert the data
  _throttle_reading = data[1] << 8 | data[2];
  _yaw_reading = data[3] << 8 | data[4];

  

  if(_throttle_reading > 40000)
  {
      _throttle_reading = _throttle_reading - 65535;  
  }
  //Serial.println(_throttle_reading);
  if(_yaw_reading > 40000)
  {
      _yaw_reading = _yaw_reading - 65535;  
  }

}

void MLX::process_right(){

  int pitch_reading;
  int roll_reading;
  
	 unsigned int data[7];
  
    _mlxI2C.beginTransmission(_right_addr);   // Start I2C Transmission
    _mlxI2C.write(0x3E);   // Start single meaurement mode, ZYX enabled
    _mlxI2C.endTransmission();   // Stop I2C Transmission
    _mlxI2C.requestFrom(_right_addr, 1);    // Request 1 byte of data
 
  if(_mlxI2C.available() == 1){   // Read status byte
    unsigned int c = _mlxI2C.read();
  }

    _mlxI2C.beginTransmission(_right_addr);   // Start I2C Transmission
    _mlxI2C.write(0x4E);   // Send read measurement command, ZYX enabled
    _mlxI2C.endTransmission();   // Stop I2C Transmission
    _mlxI2C.requestFrom(_right_addr, 7);    // Request 7 bytes of data
 
  if(_mlxI2C.available() == 7){;    // Read 7 bytes of data   status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb

    data[0] = _mlxI2C.read();
    data[1] = _mlxI2C.read();
    data[2] = _mlxI2C.read();
    data[3] = _mlxI2C.read();
    data[4] = _mlxI2C.read();
    data[5] = _mlxI2C.read();
    data[6] = _mlxI2C.read();
  }
 
// Convert the data
  _pitch_reading = data[1] << 8 | data[2];
  _roll_reading = data[3] << 8 | data[4];

  

  if(_pitch_reading > 40000)
  {
      _pitch_reading = _pitch_reading - 65535;  
  }
  
  if(_roll_reading > 40000)
  {
      _roll_reading = _roll_reading - 65535;  
  }
}

volatile int MLX::get_throttle(){
  
	return _throttle_reading;
  
}
volatile int MLX::get_yaw(){

	return _yaw_reading;
 
}
volatile int MLX::get_pitch(){

	return _pitch_reading;
  
}
volatile int MLX::get_roll(){

	return _roll_reading;
  
}
