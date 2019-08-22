// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// MLX90393
// This code is designed to work with the MLX90393_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products
 
#include<Wire.h>
 
// MLX90393 I2C Address is 0x0C(12)
#define Addr 0x0D
 
int x, y = 0;  

void setup(){

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
 
// Request 1 byte of Reading
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
 
// Request 1 byte of Reading
Wire.requestFrom(Addr, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
delay(300);
}
 
void loop(){
unsigned int Reading[7];
 
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Start single meaurement mode, ZYX enabled
Wire.write(0x3E);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of Reading
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
 
// Request 7 bytes of Reading
Wire.requestFrom(Addr, 7);
 
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
int xMag = Reading[1] * 256 + Reading[2];
int yMag = Reading[3] * 256 + Reading[4];
//xMag = constrain(xMag, -70000, 70000);
//yMag = constrain(yMag, -70000, 70000); 

// Output Reading to serial monitor


if (xMag > 50000){
  x = xMag - 65535;
}
else x = xMag;

if (yMag > 50000){
  y = yMag - 65535;
}
else y = yMag;
Serial.print("    ");
Serial.print(x);
Serial.print("    ");
Serial.println(y);

}
