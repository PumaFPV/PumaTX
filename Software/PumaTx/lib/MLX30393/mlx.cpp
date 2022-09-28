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
/* #include <Wire.h>
 */
int _throttleReading;
int _yawReading;
int _pitchReading;
int _rollReading;
int _leftAddr;
int _rightAddr; 
uint8_t _SDA;
uint8_t _SCL;
unsigned long _freqI2C;
TwoWire *_mlxI2C;

MLX::MLX(TwoWire *mlxI2C, uint8_t SDA, uint8_t SCL, unsigned long freqI2C, int leftAddr, int rightAddr)
{    
  _mlxI2C = mlxI2C; 
  _SDA = SDA,
  _SCL = SCL;
  _freqI2C = freqI2C;
  _leftAddr = leftAddr;
  _rightAddr = rightAddr;
}


void MLX::begin() 
{
  MLX::begin(true);
}

//For void setup();
void MLX::begin(bool init)
{
  if (init) {    
    _mlxI2C->begin();
    MLX::beginLeft();
    MLX::beginRight(); 
  }
}


void MLX::beginLeft() //0x0C
{	

	_mlxI2C->beginTransmission(_leftAddr);   // Start I2C Transmission
  _mlxI2C->write(0x60);   // Select Write register command
  _mlxI2C->write(0x00);   // Set AH = 0x00, BIST disabled
  _mlxI2C->write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  _mlxI2C->write(0x00);   // Select address register, (0x00 << 2)
  _mlxI2C->endTransmission();   // Stop I2C Transmission
  _mlxI2C->requestFrom(_leftAddr, 1);    // Request 1 byte of data
 
  if(_mlxI2C->available() == 1){   // Read status byte
    unsigned int c = _mlxI2C->read();
  }
 
    _mlxI2C->beginTransmission(_leftAddr);   // Start I2C Transmission
    _mlxI2C->write(0x60);   // Select Write register command
    _mlxI2C->write(0x02);   // Set AH = 0x02
    _mlxI2C->write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    _mlxI2C->write(0x08);   // Select address register, (0x02 << 2)
    _mlxI2C->endTransmission();   // Stop I2C Transmission
    _mlxI2C->requestFrom(_leftAddr, 1);    // Request 1 byte of data  

  if(_mlxI2C->available() == 1){   // Read status byte
    unsigned int c = _mlxI2C->read();
  }
}	
	
void MLX::beginRight(){	//0x0D

	_mlxI2C->beginTransmission(_rightAddr);   // Start I2C Transmission
  _mlxI2C->write(0x60);   // Select Write register command
  _mlxI2C->write(0x00);   // Set AH = 0x00, BIST disabled
  _mlxI2C->write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  _mlxI2C->write(0x00);   // Select address register, (0x00 << 2)
  _mlxI2C->endTransmission();   // Stop I2C Transmission
  _mlxI2C->requestFrom(_rightAddr, 1);    // Request 1 byte of data
 
  if(_mlxI2C->available() == 1){   // Read status byte
    unsigned int c = _mlxI2C->read();
  }
 
    _mlxI2C->beginTransmission(_rightAddr);   // Start I2C Transmission
    _mlxI2C->write(0x60);   // Select Write register command
    _mlxI2C->write(0x02);   // Set AH = 0x02
    _mlxI2C->write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    _mlxI2C->write(0x08);   // Select address register, (0x02 << 2)
    _mlxI2C->endTransmission();   // Stop I2C Transmission
    _mlxI2C->requestFrom(_rightAddr, 1);    // Request 1 byte of data  

  if(_mlxI2C->available() == 1){   // Read status byte
    unsigned int c = _mlxI2C->read();
  }
}


//For void loop();
void MLX::process(){
	
	MLX::processLeft();
	MLX::processRight();
	
}

void MLX::processLeft(){
  
  int throttleReading;
  int yawReading;
	
  unsigned int data[7];
  
    _mlxI2C->beginTransmission(_leftAddr);   // Start I2C Transmission
    _mlxI2C->write(0x3E);   // Start single meaurement mode, ZYX enabled
    _mlxI2C->endTransmission();   // Stop I2C Transmission
    _mlxI2C->requestFrom(_leftAddr, 1);    // Request 1 byte of data
 
  if(_mlxI2C->available() == 1){   // Read status byte
    unsigned int c = _mlxI2C->read();
  }

    _mlxI2C->beginTransmission(_leftAddr);   // Start I2C Transmission
    _mlxI2C->write(0x4E);   // Send read measurement command, ZYX enabled
    _mlxI2C->endTransmission();   // Stop I2C Transmission
    _mlxI2C->requestFrom(_leftAddr, 7);    // Request 7 bytes of data
 
  if(_mlxI2C->available() == 7){;    // Read 7 bytes of data   status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb

    data[0] = _mlxI2C->read();
    data[1] = _mlxI2C->read();
    data[2] = _mlxI2C->read();
    data[3] = _mlxI2C->read();
    data[4] = _mlxI2C->read();
    data[5] = _mlxI2C->read();
    data[6] = _mlxI2C->read();
  }
 
  // Convert the data
  _throttleReading = data[1] << 8 | data[2];
  _yawReading = data[3] << 8 | data[4];

  

  if(_throttleReading > 40000)
  {
      _throttleReading = _throttleReading - 65535;  
  }
  //Serial.println(_throttle_reading);
  if(_yawReading > 40000)
  {
      _yawReading = _yawReading - 65535;  
  }

}

void MLX::processRight(){

  int pitchReading;
  int rollReading;
  
	 unsigned int data[7];
  
    _mlxI2C->beginTransmission(_rightAddr);   // Start I2C Transmission
    _mlxI2C->write(0x3E);   // Start single meaurement mode, ZYX enabled
    _mlxI2C->endTransmission();   // Stop I2C Transmission
    _mlxI2C->requestFrom(_rightAddr, 1);    // Request 1 byte of data
 
  if(_mlxI2C->available() == 1){   // Read status byte
    unsigned int c = _mlxI2C->read();
  }

    _mlxI2C->beginTransmission(_rightAddr);   // Start I2C Transmission
    _mlxI2C->write(0x4E);   // Send read measurement command, ZYX enabled
    _mlxI2C->endTransmission();   // Stop I2C Transmission
    _mlxI2C->requestFrom(_rightAddr, 7);    // Request 7 bytes of data
 
  if(_mlxI2C->available() == 7){;    // Read 7 bytes of data   status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb

    data[0] = _mlxI2C->read();
    data[1] = _mlxI2C->read();
    data[2] = _mlxI2C->read();
    data[3] = _mlxI2C->read();
    data[4] = _mlxI2C->read();
    data[5] = _mlxI2C->read();
    data[6] = _mlxI2C->read();
  }
 
// Convert the data
  _pitchReading = data[1] << 8 | data[2];
  _rollReading = data[3] << 8 | data[4];

  

  if(_pitchReading > 40000)
  {
      _pitchReading = _pitchReading - 65535;  
  }
  
  if(_rollReading > 40000)
  {
      _rollReading = _rollReading - 65535;  
  }
}

volatile int MLX::getThrottle(){
  
	return _throttleReading;
  
}

volatile int MLX::getYaw(){

	return _yawReading;
 
}

volatile int MLX::getPitch(){

	return _pitchReading;
  
}

volatile int MLX::getRoll(){

	return _rollReading;
  
}
