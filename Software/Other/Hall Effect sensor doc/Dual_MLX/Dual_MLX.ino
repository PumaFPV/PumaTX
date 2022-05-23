#include<Wire.h>

#define Left 0x0C
#define Right 0x0D

int a,b,c,d = 0;

void SetupLeft(){
  // Start I2C Transmission
Wire.beginTransmission(Left);
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
 
// Request 1 byte of Reading
Wire.requestFrom(Left, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
 
// Start I2C Transmission
Wire.beginTransmission(Left);
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
 
// Request 1 byte of Reading
Wire.requestFrom(Left, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
delay(300);
}

void SetupRight(){
  // Start I2C Transmission
Wire.beginTransmission(Right);
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
 
// Request 1 byte of Reading
Wire.requestFrom(Right, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
 
// Start I2C Transmission
Wire.beginTransmission(Right);
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
 
// Request 1 byte of Reading
Wire.requestFrom(Right, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
delay(300);
}

void setup(){

  Wire.begin(); // Initialise I2C communication as MASTER
  Serial.begin(9600); // Initialise serial communication, set baud rate = 9600
  
  SetupLeft();
  SetupRight();
}



void LeftLoop(){

unsigned int Reading[7];
 
// Start I2C Transmission
Wire.beginTransmission(Left);
// Start single meaurement mode, ZYX enabled
Wire.write(0x3E);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of Reading
Wire.requestFrom(Left, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}

 
// Start I2C Transmission
Wire.beginTransmission(Left);
// Send read measurement command, ZYX enabled
Wire.write(0x4E);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 7 bytes of Reading
Wire.requestFrom(Left, 7);
 
// Read 7 bytes of Reading
// status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
if(Wire.available() == 7);
{
Reading[0] = Wire.read();
Reading[1] = Wire.read();
Reading[2] = Wire.read();
Reading[3] = Wire.read();
Reading[4] = Wire.read();
Reading[5] = Wire.read();
Reading[6] = Wire.read();
Reading[7] = Wire.read();
}
 
// Convert the Reading
int aMag = Reading[1] * 256 + Reading[2];
int bMag = Reading[3] * 256 + Reading[4];

// Output Reading to serial monitor

if (aMag > 50000){
  a = aMag - 65535;
}
else a = aMag;

if (bMag > 50000){
  b = bMag - 65535;
}
else b = bMag;
}


void RightLoop(){
  
unsigned int Reading[7];
 
// Start I2C Transmission
Wire.beginTransmission(Right);
// Start single meaurement mode, ZYX enabled
Wire.write(0x3E);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of Reading
Wire.requestFrom(Right, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}

 
// Start I2C Transmission
Wire.beginTransmission(Right);
// Send read measurement command, ZYX enabled
Wire.write(0x4E);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 7 bytes of Reading
Wire.requestFrom(Right, 7);
 
// Read 7 bytes of Reading
// status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
if(Wire.available() == 7);
{
Reading[0] = Wire.read();
Reading[1] = Wire.read();
Reading[2] = Wire.read();
Reading[3] = Wire.read();
Reading[4] = Wire.read();
Reading[5] = Wire.read();
Reading[6] = Wire.read();
Reading[7] = Wire.read();
}
 
// Convert the Reading
int cMag = Reading[1] * 256 + Reading[2];
int dMag = Reading[3] * 256 + Reading[4];

// Output Reading to serial monitor


if (cMag > 50000){
  c = cMag - 65535;
}
else c = cMag;

if (dMag > 50000){
  d = dMag - 65535;
}
else d = dMag;
}


void loop(){

  RightLoop();
  LeftLoop();
  
Serial.print(a);
Serial.print("    ");
Serial.print(b);
Serial.print("    ");
Serial.print(c);
Serial.print("    ");
Serial.println(d);
}
