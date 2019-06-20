#include "U8glib.h"
#include<Wire.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

#define RC_CHANNEL_MIN 990
#define RC_CHANNEL_MAX 2010

#define SBUS_MIN_OFFSET 173
#define SBUS_MID_OFFSET 992
#define SBUS_MAX_OFFSET 1811
#define SBUS_CHANNEL_NUMBER 16
#define SBUS_PACKET_LENGTH 25
#define SBUS_FRAME_HEADER 0x0f
#define SBUS_FRAME_FOOTER 0x00
#define SBUS_FRAME_FOOTER_V2 0x04
#define SBUS_STATE_FAILSAFE 0x08
#define SBUS_STATE_SIGNALLOSS 0x04
#define SBUS_UPDATE_RATE 15 //ms  

uint8_t sbusPacket[SBUS_PACKET_LENGTH];
int rcChannels[SBUS_CHANNEL_NUMBER];
uint32_t sbusTime = 0;

//--------------------------------------------------------------------Inputs-------------------------------------------
const int inmenuright = 2;
const int inmenuleft = 3;
const int inmenuup = 4;
const int inmenudown = 5;
const int inmenuok = 6;
int inputvoltage = A7;

int throttle, yaw, pitch, roll = 0;   //Reaading from MLX

int trimthrottle,trimyaw, trimpitch, trimroll, loopcount = 0;    //trims values

int t, y, p, r = 0;   //Mapped outputs

int outputthrottle, outputyaw, outputpitch, outputroll = 1;   //Output

int menurightstate, menuleftstate, menuupstate, menudownstate, menuokstate;
int lastmenuright, lastmenuleft, lastmenuup, lastmenudown, lastmenuok = LOW;
int calibrate = 0;

unsigned long lastdebouncetime = 0;
unsigned long debouncedelay = 50;

int page = 0;

//------------------------------------------------Min--Max--Value---------------------------------------
/*int minthrottle = -8100;
int maxthrottle = 8700;
int minyaw = -8100;
int maxyaw = 9550;
int minpitch = -8200;
int maxpitch = 9000;
int minroll = -9300;
int maxroll = 8750;
*/
int minthrottle, maxthrottle, minyaw, maxyaw, minpitch, maxpitch, minroll, maxroll =0;

float averagethrottle, averageyaw, averagepitch, averageroll = 0;
//-----------------------------------------------GRAPH------------------------------------------

int xgraph1, xgraph2, ygraph1, ygraph2 = 0;
float voltage;

//-----------------------------------------------BITMAP---------------------------------------------------

const unsigned char PROGMEM screen1 [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x07, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x19, 0xB0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x05, 0xA3, 0x00, 0x00, 0x00, 0x00, 0x72, 0xD8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x0D, 0xB1, 0x80, 0x00, 0x00, 0x00, 0xC6, 0xD8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x09, 0x18, 0xC0, 0x07, 0xF0, 0x01, 0x8C, 0x48, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x19, 0x0C, 0x31, 0xFF, 0xFF, 0xE6, 0x08, 0x4C, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1B, 0x04, 0x1F, 0xC0, 0x81, 0xFC, 0x10, 0x6C, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x13, 0x02, 0xEE, 0x00, 0x80, 0x3B, 0xB0, 0x64, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x12, 0x03, 0x8F, 0x80, 0x80, 0xFC, 0xE0, 0x64, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x18, 0x1F, 0x1C, 0xC0, 0x81, 0x9C, 0x7C, 0x6C, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x0E, 0x76, 0x36, 0x60, 0x83, 0x36, 0x67, 0x38, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x07, 0xC6, 0x27, 0x30, 0x86, 0x76, 0x71, 0xF0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x0D, 0x2F, 0x0C, 0x98, 0x52, 0x58, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x02, 0x0D, 0x65, 0x06, 0xB0, 0x53, 0x48, 0x20, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x19, 0x45, 0x81, 0xE0, 0xD1, 0x4C, 0x60, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x19, 0xC5, 0x87, 0xF0, 0xD1, 0xCC, 0x60, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x91, 0x84, 0xFE, 0x9F, 0x90, 0xC4, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0xB1, 0x84, 0xC0, 0x81, 0x90, 0xC6, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0xA3, 0x84, 0xC0, 0x80, 0x80, 0xF2, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xEE, 0x85, 0x80, 0x81, 0xD0, 0x9B, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x85, 0x40, 0x81, 0x51, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC3, 0x60, 0x81, 0x71, 0x81, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x43, 0x60, 0x83, 0x71, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x66, 0x60, 0x83, 0x33, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x9F, 0xF6, 0x20, 0x82, 0x37, 0xFC, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xC6, 0x20, 0x82, 0x31, 0x8F, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x83, 0x20, 0x82, 0x60, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC1, 0x83, 0x00, 0x86, 0x60, 0xC1, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC1, 0xFD, 0x30, 0x86, 0x5F, 0xC1, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xE3, 0x7F, 0x90, 0x86, 0xFF, 0x61, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0x67, 0xD8, 0x8C, 0xF2, 0x73, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFB, 0x78, 0xDF, 0xFD, 0x8F, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x79, 0xF3, 0xE7, 0xCF, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x8F, 0x60, 0xC7, 0xF8, 0xE6, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x8C, 0x61, 0x43, 0x18, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x6D, 0x0C, 0x23, 0x62, 0x18, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x06, 0x66, 0x33, 0x3C, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x1A, 0x64, 0x11, 0x2C, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x1B, 0x4C, 0x19, 0xAC, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x9B, 0xC8, 0x08, 0xE4, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x91, 0x98, 0x04, 0xC4, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xF1, 0xB0, 0x07, 0xC7, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x33, 0x20, 0x03, 0x66, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x32, 0x20, 0x02, 0x36, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x60, 0x03, 0x17, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x60, 0x03, 0x1D, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x60, 0x03, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x68, 0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x3F, 0xFE, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x33, 0xE6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xDC, 0x0F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x8E, 0x18, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x06, 0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03, 0xE0, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0xC0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x8F, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//-------------------------------------------------------SETUP--------------------------------------------------------------
void setup(){

  Wire.begin();   // Initialise I2C communication as MASTER
    Serial.begin(9600);   // Initialise serial communication, set baud rate = 9600
    for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        rcChannels[i] = 1500;
    }
    //Serial.begin(100000, SERIAL_8E2);
  
//--------------------------------------------------I2C-0x0C-LEFT-GIMBAL----------------------------------------------------

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

//--------------------------------------------------I2C-0x0C-RIGHT-GIMBAL----------------------------------------------------

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

//--------------------------------------------------MENU-------------------------------------------------
  pinMode(inmenuright, INPUT);
  pinMode(inmenuleft, INPUT);
  pinMode(inmenuup, INPUT);
  pinMode(inmenudown, INPUT);
  pinMode(inmenuok, INPUT);
  
  //delay(300);
}




//-----------------------------------------------LOOP------------------------------------------------------------------------------------------------------------------------
void loop(){

  unsigned int data[7];
 
//--------------------------------------------------I2C-0x0C-LEFT-GIMBAL----------------------------------------------------
    Wire.beginTransmission(0x0C);   // Start I2C Transmission
    Wire.write(0x3E);   // Start single meaurement mode, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
  //delay(100);
 

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
throttle = data[1] * 256 + data[2];
yaw = data[3] * 256 + data[4];
 
// Output data to serial monitor
//Serial.print("Throttle: ");
//Serial.print(Throttle);
//Serial.print("Yaw: ");
//Serial.print(Yaw);

//--------------------------------------------------I2C-0x0D-RIGHT-GIMBAL----------------------------------------------------
    Wire.beginTransmission(0x0D);   // Start I2C Transmission
    Wire.write(0x3E);   // Start single meaurement mode, ZYX enabled
    Wire.endTransmission();   // Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);    // Request 1 byte of data
 
  if(Wire.available() == 1){   // Read status byte
    unsigned int c = Wire.read();
  }
  //delay(100);
 

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
pitch = data[1] * 256 + data[2];
roll = data[3] * 256 + data[4];
 
// Output data to serial monitor
//Serial.print("Pitch: ");
//Serial.print(Pitch);
//Serial.print("Roll: ");
//Serial.println(Roll);

//--------------------------------------LOOP-SCREEN-----------------------------------------------------
  u8g.firstPage();  
    do {
      draw();
  } while( u8g.nextPage() );
  

//delay(1);

//--------------------------LOOP--S-BUS-------------------------------------------------------
/*
    uint32_t currentMillis = millis();

    
     //Here you can modify values of rcChannels while keeping it in 1000:2000 range
     

    if (currentMillis > sbusTime) {
        sbusPreparePacket(sbusPacket, rcChannels, false, false);
        Serial.write(sbusPacket, SBUS_PACKET_LENGTH);

        sbusTime = currentMillis + SBUS_UPDATE_RATE;
    }
*/

//------------------------------------------------------------------------------------------MENU-----------------------------------------------

int readingmenuok = digitalRead(inmenuok);
int readingmenuright = digitalRead(inmenuright);
int readingmenuleft = digitalRead(inmenuleft);
int readingmenuup = digitalRead(inmenuup);
int readingmenudown = digitalRead(inmenudown);


//-----------------------------------------------------------------------------------------FILTRATION---------------------------------------------

//Average Method 
/*
for(int i = 0; i < 200; ++i)
  {
    averagethrottle += Throttle;
    delay(1);
  }
  averagethrottle /= 200;
  */


//-----------------------------------------------------------------------------------------CALIBRATION--------------------------------------------


if (page == 1){   //Calibrate procedure begin
  //Serial.print(readingmenuok);
  //Serial.print("  ");
  
 if (readingmenuok == LOW) {
    lastdebouncetime = millis();
 }
  if (millis() - lastdebouncetime > 2000){
    calibrate = 1;

    //Calibrate trims
    trimthrottle = throttle;  
    trimyaw = yaw;
    trimpitch = pitch;
    trimroll = roll;  

    //Calibrate min/max----------Throttle
    if (throttle > maxthrottle){
      maxthrottle = throttle - 20;
    }
    if(throttle < minthrottle){
      minthrottle = throttle + 20;
    }
    
    //Calibrate min/max----------Yaw
    if (yaw > maxyaw){
      maxyaw = yaw - 20;
    }
    if (yaw < minyaw){
      minyaw = yaw + 20;
    }

    //Calibrate min/max----------Pitch
    if (pitch > maxpitch){
      maxpitch = pitch - 20;
    }
    if (pitch < minpitch){
      minpitch = pitch + 20;
    }

    //Calibrate min/max----------Roll
    if (roll > maxroll){
      maxroll = roll - 20;
    }
    if (roll < minroll){
      minroll = roll + 20;
    }
    
    
  } 
  //Serial.println(calibrate);
}
//reset calibrate procedure
if (readingmenuok == HIGH){
    calibrate = 0;
}





//-------------------------------------------------------------------TRIMS-------------------------------------------------------------------------------




//------------------------------------------------------RC-----------------------------------------------

if (throttle <= trimthrottle) {    //-----------------------------------Throttle
  outputthrottle = map(throttle, minthrottle, trimthrottle, -100, 0);
}
else if (throttle > trimthrottle) {
  outputthrottle = map(throttle, trimthrottle, maxthrottle, 0, 100);
}

if (yaw <= trimyaw) {    //-----------------------------------yaw
  outputyaw = map(yaw, minyaw, trimyaw, -100, 0);
}
else if (yaw > trimyaw) {
  outputyaw = map(yaw, trimyaw, maxyaw, 0, 100);
}

if (pitch <= trimpitch) {    //-----------------------------------pitch
  outputpitch = map(pitch, minpitch, trimpitch, -100, 0);
}
else if (pitch > trimpitch) {
  outputpitch = map(pitch, trimpitch, maxpitch, 0, 100);
}

if (roll <= trimroll) {    //-----------------------------------roll
  outputroll = map(roll, minroll, trimroll, -100, 0);
}
else if (roll > trimroll) {
  outputroll = map(roll, trimroll, maxroll, 0, 100);
}

t = map(throttle, minthrottle, maxthrottle, -100, 100);
y = map(yaw, minyaw, maxyaw, 100, -100);
p = map(pitch, minpitch, maxpitch, -100, 100);
r = map(roll, minroll, maxroll, 100, -100);


//---------------------------------------------------------------NAVIGATION--------------------------------------------------------

//Simple method
if (readingmenuright == HIGH && page < 5) { //menu right -> page+
  ++page;
}

if (readingmenuleft == HIGH && page > 0){ //menu left -> page-
  --page;
}


//---------------------------------------------------------------------------Page-3-fan-control------------------------------
 voltage = analogRead(inputvoltage) * ( 5.00 / 1023.00) / 0.4443;
  Serial.println(voltage);

} //Void loop end

//---------------------------------------------------------------------------DRAW-PAGE---------------------------------------------------------------------------
void draw(void) {
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(123, 57);
  u8g.print(page);

if (page == 0){ //-----------------------------------------------------------Page-0------------------------------
  u8g.drawBitmapP( 0, 0, 16, 64, screen1);
}

if (page == 1){ //-----------------------------------------------------------Page-1------------------------------
  
  //Throttle
  u8g.setPrintPos(0, 8); 
  u8g.print("Throttle:");
  u8g.setPrintPos(0, 16);
  u8g.print(throttle);
  u8g.setPrintPos(0, 24);
  u8g.print(outputthrottle);
  
  //Yaw
  u8g.setPrintPos(0, 40); 
  u8g.print("Yaw:");
  u8g.setPrintPos(0, 48);
  u8g.print(yaw);
  u8g.setPrintPos(0, 56);
  u8g.print(outputyaw);
  
  //Pitch
  u8g.setPrintPos(64, 8); 
  u8g.print("Pitch:");
  u8g.setPrintPos(64, 16);
  u8g.print(pitch);
  u8g.setPrintPos(64, 24);
  u8g.print(outputpitch);
  
  //Roll
  u8g.setPrintPos(64, 40); 
  u8g.print("Roll:");
  u8g.setPrintPos(64, 48);
  u8g.print(roll);
  u8g.setPrintPos(64, 56);
  u8g.print(outputroll);



}

if (page == 2){ //---------------------------------------------------------------Page-2------------------

  //Left Joystick
  xgraph1 = map(y, -100, 100, 8, 56);   
  ygraph1 = map(t, 100, -100, 8, 56);
  u8g.drawCircle(32, 32, 20);
  u8g.drawCircle(xgraph1, ygraph1, 2);
  u8g.drawLine(32,32,xgraph1,ygraph1);
  
  //Right Joystick
  xgraph2 = map(r, -100, 100, 72, 120);  
  ygraph2 = map(p, 100, -100, 8, 56);
  u8g.drawCircle(96, 32, 20);
  u8g.drawCircle(xgraph2, ygraph2, 2);
  u8g.drawLine(96,32,xgraph2,ygraph2);

  /*Serial.print(xgraph1);
  Serial.print(" ");
  Serial.print(ygraph1);
  Serial.print(" ");
  Serial.print(xgraph2);
  Serial.print(" ");
  Serial.println(ygraph2);
  */

 
  
  
}


if (page == 3){ //---------------------------------------------------------------Page-3--------------------
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 11);
  u8g.print("Setup");


  //Voltage
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(0, 50);
  u8g.print("Voltage:");
  u8g.setPrintPos(.0, 57);
  u8g.print(voltage);

}

} //void end





//------------------------------------------SBUS---------------------------------------

/*


void sbusPreparePacket(uint8_t packet[], int channels[], bool isSignalLoss, bool isFailsafe){

    static int output[SBUS_CHANNEL_NUMBER] = {0};

    
     //Map 1000-2000 with middle at 1500 chanel values to
     //173-1811 with middle at 992 S.BUS protocol requires
     
    for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        output[i] = map(channels[i], RC_CHANNEL_MIN, RC_CHANNEL_MAX, SBUS_MIN_OFFSET, SBUS_MAX_OFFSET);
    }

    uint8_t stateByte = 0x00;
    if (isSignalLoss) {
        stateByte |= SBUS_STATE_SIGNALLOSS;
    }
    if (isFailsafe) {
        stateByte |= SBUS_STATE_FAILSAFE;
    }
    packet[0] = SBUS_FRAME_HEADER; //Header

    packet[1] = (uint8_t) (output[0] & 0x07FF);
    packet[2] = (uint8_t) ((output[0] & 0x07FF)>>8 | (output[1] & 0x07FF)<<3);
    packet[3] = (uint8_t) ((output[1] & 0x07FF)>>5 | (output[2] & 0x07FF)<<6);
    packet[4] = (uint8_t) ((output[2] & 0x07FF)>>2);
    packet[5] = (uint8_t) ((output[2] & 0x07FF)>>10 | (output[3] & 0x07FF)<<1);
    packet[6] = (uint8_t) ((output[3] & 0x07FF)>>7 | (output[4] & 0x07FF)<<4);
    packet[7] = (uint8_t) ((output[4] & 0x07FF)>>4 | (output[5] & 0x07FF)<<7);
    packet[8] = (uint8_t) ((output[5] & 0x07FF)>>1);
    packet[9] = (uint8_t) ((output[5] & 0x07FF)>>9 | (output[6] & 0x07FF)<<2);
    packet[10] = (uint8_t) ((output[6] & 0x07FF)>>6 | (output[7] & 0x07FF)<<5);
    packet[11] = (uint8_t) ((output[7] & 0x07FF)>>3);
    packet[12] = (uint8_t) ((output[8] & 0x07FF));
    packet[13] = (uint8_t) ((output[8] & 0x07FF)>>8 | (output[9] & 0x07FF)<<3);
    packet[14] = (uint8_t) ((output[9] & 0x07FF)>>5 | (output[10] & 0x07FF)<<6);  
    packet[15] = (uint8_t) ((output[10] & 0x07FF)>>2);
    packet[16] = (uint8_t) ((output[10] & 0x07FF)>>10 | (output[11] & 0x07FF)<<1);
    packet[17] = (uint8_t) ((output[11] & 0x07FF)>>7 | (output[12] & 0x07FF)<<4);
    packet[18] = (uint8_t) ((output[12] & 0x07FF)>>4 | (output[13] & 0x07FF)<<7);
    packet[19] = (uint8_t) ((output[13] & 0x07FF)>>1);
    packet[20] = (uint8_t) ((output[13] & 0x07FF)>>9 | (output[14] & 0x07FF)<<2);
    packet[21] = (uint8_t) ((output[14] & 0x07FF)>>6 | (output[15] & 0x07FF)<<5);
    packet[22] = (uint8_t) ((output[15] & 0x07FF)>>3);

    packet[23] = stateByte; //Flags byte
    packet[24] = SBUS_FRAME_FOOTER; //Footer
}
*/





