/*
	MLX.cpp - Library for reading 2 mlx30393 hall sensors
	0x0C is Left gimbal
	0x0D is Right gimbal
	
	Ask for MLX.begin(); in void setup and MLX.process(); in loop
	Then get value from = MLX.Throttle();
	
	Created by Nicolas de Grave, May 19, 2019
*/

#include "Arduino.h"
#include "MLX.h"
#include <Wire.h>


/*void MLX::begin(addr){	//Trying to compact stuff a little more (only one begin)
 Wire.beginTransmission(addr);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x00);   // Set AH = 0x00, BIST disabled
    Wire.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);   // Select address register, (0x00 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(addr, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
 
    Wire.beginTransmission(addr);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x02);   // Set AH = 0x02
    Wire.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);   // Select address register, (0x02 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(addr, 1);    // Request 1 byte of data  

  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
}
*/

//For void setup();
void MLX::begin(){
	
	MLX::beginLeft();
	MLX::beginRight();	
	
}
void MLX::beginLeft(){	//0x0C

Wire.beginTransmission(0x0C);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x00);   // Set AH = 0x00, BIST disabled
    Wire.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);   // Select address register, (0x00 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
 
    Wire.beginTransmission(0x0C);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x02);   // Set AH = 0x02
    Wire.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);   // Select address register, (0x02 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);    // Request 1 byte of data  

  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
}	
	
void MLX::beginRight(){	//0x0D

	Wire.beginTransmission(0x0D);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x00);   // Set AH = 0x00, BIST disabled
    Wire.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);   // Select address register, (0x00 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
 
    Wire.beginTransmission(0x0D);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x02);   // Set AH = 0x02
    Wire.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);   // Select address register, (0x02 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);    // Request 1 byte of data  

  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
}


//For void loop();
void MLX::process(){
	
	MLX::processLeft();
	MLX::processRight();
	
}
void MLX::processLeft(){
  
  int Throttle;
  int Yaw;
	
	 unsigned int data[7];
  
    Wire.beginTransmission(0x0C);   // Start I2C Transmission
    Wire.write(0x3E);   // Start single meaurement mode, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }

    Wire.beginTransmission(0x0C);   // Start I2C Transmission
    Wire.write(0x4E);   // Send read measurement command, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0C, 7);    // Request 7 bytes of data
 
  if(Wire.available() == 7){;    // Read 7 bytes of data   status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb

    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }
 
// Convert the data
Throttle = data[1] * 256 + data[2];
Yaw = data[3] * 256 + data[4];

}
void MLX::processRight(){

  int Pitch;
  int Roll;
  
	 unsigned int data[7];
  
    Wire.beginTransmission(0x0D);   // Start I2C Transmission
    Wire.write(0x3E);   // Start single meaurement mode, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }

    Wire.beginTransmission(0x0D);   // Start I2C Transmission
    Wire.write(0x4E);   // Send read measurement command, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0D, 7);    // Request 7 bytes of data
 
  if(Wire.available() == 7){;    // Read 7 bytes of data   status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb

    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }
 
// Convert the data
Pitch = data[1] * 256 + data[2];
Roll = data[3] * 256 + data[4];

}

//To read data
int MLX::getThrottle(){
  
	return Throttle;
  
}
int MLX::getYaw(){

	return Yaw;
 
}
int MLX::getPitch(){

	return Pitch;
  
}
int MLX::getRoll(){

	return Roll;
  
}
