#define LeftAddr 0x0C
#define RightAddr 0x0D

#include <Wire.h>

void setup(){
  Wire.beginTransmission(LeftAddr);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x00);   // Set AH = 0x00, BIST disabled
    Wire.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);   // Select address register, (0x00 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(LeftAddr, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
 
    Wire.beginTransmission(LeftAddr);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x02);   // Set AH = 0x02
    Wire.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);   // Select address register, (0x02 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(LeftAddr, 1);    // Request 1 byte of data  

  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
Wire.beginTransmission(RightAddr);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x00);   // Set AH = 0x00, BIST disabled
    Wire.write(0x5C);   // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);   // Select address register, (0x00 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(RightAddr, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
 
    Wire.beginTransmission(RightAddr);   // Start I2C Transmission
    Wire.write(0x60);   // Select Write register command
    Wire.write(0x02);   // Set AH = 0x02
    Wire.write(0xB4);   // Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);   // Select address register, (0x02 << 2)
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(RightAddr, 1);    // Request 1 byte of data  

  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
  
}

void loop(){
  
   unsigned int data[7];
  
    Wire.beginTransmission(LeftAddr);   // Start I2C Transmission
    Wire.write(0x3E);   // Start single meaurement mode, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(LeftAddr, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }

    Wire.beginTransmission(LeftAddr);   // Start I2C Transmission
    Wire.write(0x4E);   // Send read measurement command, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(LeftAddr, 7);    // Request 7 bytes of data
 
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
ThrottleReading = data[1];// * 256 + data[2];
YawReading = data[3];// * 256 + data[4];
  
   unsigned int data[7];
  
    Wire.beginTransmission(RightAddr);   // Start I2C Transmission
    Wire.write(0x3E);   // Start single meaurement mode, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(RightAddr, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }

    Wire.beginTransmission(RightAddr);   // Start I2C Transmission
    Wire.write(0x4E);   // Send read measurement command, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(RightAddr, 7);    // Request 7 bytes of data
 
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
PitchReading = data[1];// * 256 + data[2];
RollReading = data[3];// * 256 + data[4];

}
}
