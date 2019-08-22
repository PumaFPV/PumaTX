#include<Wire.h>

#define Left 0x0C
#define Right 0x0D

int a,b,c,d = 0;

//this programm will put out a PPM signal

//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 8  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity: 1 is positive, 0 is negative
#define sigPin 35  //set PPM signal pin on the arduino
//////////////////////////////////////////////////////////////////


/*this array holds the servo values for the ppm signal
 change theese values in your code (usually servo values are between 1000 and 2000)*/
int ppm[chanel_number];


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
  //Serial.begin(9600); // Initialise serial communication, set baud rate = 9600
  
  SetupLeft();
  SetupRight();
  
  //initiallize default ppm values
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
  }

  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)
  
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


void ProcessPPM(){
ppm[1] = map(a, -9000, 9000, 1000, 2000);
ppm[2] = map(b, -9000, 9000, 1000, 2000);
ppm[3] = map(c, -9000, 9000, 1000, 2000);
ppm[4] = map(d, -9000, 9000, 1000, 2000);
}


void ppmWrite(){  //generate PPM signal
  static unsigned long lastFrLen;
  static unsigned long lastServo;
  static unsigned long lastPulse;
  static boolean PPM_run;
  static boolean pulse;
  static boolean pulseStart = true;
  static byte counter;
  static byte part = true;

  if(micros() - lastFrLen >= PPM_FrLen){  //start PPM signal after PPM_FrLen has passed
    lastFrLen = micros();
    PPM_run = true;
  }

  if(counter >= chanel_number){
    PPM_run = false;
    counter = 0;
    pulse = true;  //put out the last pulse
  }

  if(PPM_run){
    if (part){  //put out the pulse
      pulse = true;
      part = false;
      lastServo = micros();
    }
    else{  //wait till servo signal time (values from the ppm array) has passed
      if(micros() - lastServo >= ppm[counter]){
        counter++;  //do the next channel
        part = true;
      }
    }
  }

  if(pulse){
    if(pulseStart == true){  //start the pulse
      digitalWrite(sigPin, onState);
      pulseStart = false;
      lastPulse = micros();
    }
    else{  //will wait till PPM_PulseLen has passed and finish the pulse
      if(micros() - lastPulse >= PPM_PulseLen){
        digitalWrite(sigPin, !onState);
        pulse = false;
        pulseStart = true;
      }
    }
  }
}


void loop(){

  RightLoop();
  LeftLoop();
  ProcessPPM(); 
  ppmWrite();

/*
Serial.print(ppm[1]);
Serial.print("    ");
Serial.print(ppm[2]);
Serial.print("    ");
Serial.print(ppm[3]);
Serial.print("    ");
Serial.println(ppm[4]);
*/

}
