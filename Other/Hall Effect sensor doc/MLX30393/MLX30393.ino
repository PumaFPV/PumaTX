#include<Wire.h>

// MLX90393 I2C Address is 0x0C(12)
#define Addr 0x0C

void setup() 
{
  // Initialise I2C communication as MASTER 
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select Write register command
  Wire.write(0x60);
  // Set AH = 0x00, BIST disabled
  Wire.write(0x00);
  // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5  
  Wire.write(0x5C);
  // Select address register, (0x00 << 2)
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read status byte
  if(Wire.available() == 1) 
  {
    unsigned int c = Wire.read();
  }

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select Write register command
  Wire.write(0x60);
  // Set AH = 0x02
  Wire.write(0x02);
  // Set AL = 0xB4, RES for magnetic measurement = 0
  Wire.write(0xB4);
  // Select address register, (0x02 << 2)
  Wire.write(0x08);
  // Stop I2C Transmission
  Wire.endTransmission();
  
  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read status byte
  if(Wire.available() == 1) 
  {
   unsigned int c = Wire.read();
  }
  delay(300);
}

void loop() 
{
  unsigned int data[7];
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Start single meaurement mode,  ZYX enabled
  Wire.write(0x3E);
  // Stop I2C Transmission
  Wire.endTransmission();
  
  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);
  
  // Read status byte
  if(Wire.available() == 1) 
  {
   unsigned int c = Wire.read();
  }
  delay(100);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Send read measurement command, ZYX enabled
  Wire.write(0x4E);
  // Stop I2C Transmission
  Wire.endTransmission();
  
  // Request 7 bytes of data
  Wire.requestFrom(Addr, 7);

  // Read 7 bytes of data
  // status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
  if(Wire.available() == 7);
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }

  // Convert the data
  int xMag = data[1] * 256 + data[2];
  int yMag = data[3] * 256 + data[4];
  int zMag = data[5] * 256 + data[6];

  // Output data to serial monitor
  Serial.print("Magnetic Field in X-Axis : ");
  Serial.print(xMag);
  Serial.print("Magnetic Field in Y-Axis : ");
  Serial.print(yMag);
  Serial.print("Magnetic Field in Z-Axis : ");
  Serial.println(zMag);
}
