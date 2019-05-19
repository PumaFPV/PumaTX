/*TO-DO
 * Make S-Bus work
 * Clean code (variable at beginning)
 * Make calibration easier (push ok for 3s and have 5s to calibrate)
 * 
 */

#include "U8glib.h"
#include <Wire.h>
#include <EEPROM.h>

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

//--------------------------------------------------Inputs--------------------------------------------------
//digital pins
#define inmenuright 2
#define inmenuleft 3
#define inmenuup 4
#define inmenudown 5
#define inmenuok 6
#define inputvoltage A7
#define sigPin 13

//--------------------------------------------------Variables--------------------------------------------------
int throttle, yaw, pitch, roll = 0;   //Reading from MLX

int trimthrottle, trimyaw, trimpitch, trimroll, loopcount = 0;    //trims values

//int t, y, p, r = 0;   //Mapped outputs from MLX

int outputthrottle, outputyaw, outputpitch, outputroll = 1;   //Output
int ppmthrottle, ppmyaw, ppmpitch, ppmroll = 1500; //ms

int calibrate = 0;

unsigned long lastdebouncetime = 0;
unsigned long debouncedelay = 50;
unsigned long currenttime = 0;

int page = 0;
int MaxPage = 3;
//--------------------------------------------------Min--Max--Value--------------------------------------------------
#define eepromaddrminthrottle 1    //EEPROM ADDRESS
#define eepromaddrmaxthrottle 2
#define eepromaddrminyaw 3
#define eepromaddrmaxyaw 4
#define eepromaddrminpitch 5
#define eepromaddrmaxpitch 6
#define eepromaddrminroll 7
#define eepromaddrmaxroll 8
#define eepromaddrtrimthrottle 9
#define eepromaddrtrimyaw 10
#define eepromaddrtrimpitch 11
#define eepromaddrtrimroll 12

int eepromvalminthrottle, eepromvalmaxthrottle, eepromvalminyaw, eepromvalmaxyaw, eepromvalminpitch, eepromvalmaxpitch, eepromvalminroll, eepromvalmaxroll, eepromvaltrimyaw, eepromvaltrimthrottle, eepromvaltrimpitch, eepromvaltrimroll = 0;

int minthrottle, maxthrottle, minyaw, maxyaw, minpitch, maxpitch, minroll, maxroll = 0;
int minhallsensorval = -10000;
int maxhallsensorval = 10000;

//--------------------------------------------------AVERAGE--------------------------------------------------
float averagethrottle, averageyaw, averagepitch, averageroll = 0;

//--------------------------------------------------GRAPH--------------------------------------------------
int xgraph1, xgraph2, ygraph1, ygraph2 = 0;
float voltage;

//--------------------------------------------------PPM--------------------------------------------------
#define CHANNEL_NUMBER 8  //set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1500  //set the default servo value
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
//Showed at the beginning for config purpose #define sigPin 13  //set PPM signal output pin on the arduino

int ppm[CHANNEL_NUMBER]; /*this array holds the servo values for the ppm signal change theese values in your code (usually servo values move between 1000 and 2000)*/

//--------------------------------------------------BITMAP--------------------------------------------------
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

//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void setup(){

  Wire.begin();   // Initialise I2C communication as MASTER
  Serial.begin(9600);   // Initialise serial communication, set baud rate = 9600

//--------------------------------------------------ShowSketchName--------------------------------------------------
    String path = __FILE__;
    int slash = path.lastIndexOf('\x5C');
    String the_cpp_name = path.substring(slash+1);
    int dot_loc = the_cpp_name.lastIndexOf('.');
    String Firmware = the_cpp_name.substring(0, dot_loc);
    Serial.println(Firmware);


//--------------------------------------------------SBUS--------------------------------------------------
    /*for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        rcChannels[i] = 1500;
    }
    Serial1.begin(100000, SERIAL_8E2);  //SERIAL SBUS 
  */
  
//--------------------------------------------------PPM--------------------------------------------------
/*  
  //initiallize default ppm values
  for(int i=0; i<CHANNEL_NUMBER; i++){
    ppm[i]= CHANNEL_DEFAULT_VALUE;
  }
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)
  
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
*/
//--------------------------------------------------I2C-0x0C-LEFT-GIMBAL--------------------------------------------------
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

//--------------------------------------------------MENU--------------------------------------------------
  pinMode(inmenuright, INPUT);
  pinMode(inmenuleft, INPUT);
  pinMode(inmenuup, INPUT);
  pinMode(inmenudown, INPUT);
  pinMode(inmenuok, INPUT);

//--------------------------------------------------READ-CALIBRATION-FROM-EEPROM--------------------------------------------------
  minthrottle = map(EEPROM.read(eepromaddrminthrottle), 0, 255, minhallsensorval, maxhallsensorval);
  maxthrottle = map(EEPROM.read(eepromaddrmaxthrottle), 0, 255, minhallsensorval, maxhallsensorval);
  trimthrottle = map(EEPROM.read(eepromaddrtrimthrottle), 0, 255, minhallsensorval, maxhallsensorval);
  minyaw = map(EEPROM.read(eepromaddrminyaw), 0, 255, minhallsensorval, maxhallsensorval);
  maxyaw = map(EEPROM.read(eepromaddrmaxyaw), 0, 255, minhallsensorval, maxhallsensorval);
  trimyaw = map(EEPROM.read(eepromaddrtrimyaw), 0, 255, minhallsensorval, maxhallsensorval);
  minpitch = map(EEPROM.read(eepromaddrminpitch), 0, 255, minhallsensorval, maxhallsensorval);
  maxpitch = map(EEPROM.read(eepromaddrmaxpitch), 0, 255, minhallsensorval, maxhallsensorval);
  trimpitch = map(EEPROM.read(eepromaddrtrimpitch), 0, 255, minhallsensorval, maxhallsensorval);
  minroll = map(EEPROM.read(eepromaddrminroll), 0, 255, minhallsensorval, maxhallsensorval);
  maxroll = map(EEPROM.read(eepromaddrmaxroll), 0, 255, minhallsensorval, maxhallsensorval);
  trimroll = map(EEPROM.read(eepromaddrtrimroll), 0, 255, minhallsensorval, maxhallsensorval);
  
}
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------END-VOID-SETUP---------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================








//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-LOOP--------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void loop(){
  

//--------------------------------------------------READ-I2C-0x0C-LEFT-GIMBAL--------------------------------------------------
  unsigned int data[7];
  
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
//Serial.print("throttle: ");
//Serial.print(throttle);
//Serial.print("yaw: ");
//Serial.print(yaw);

//--------------------------------------------------READ-I2C-0x0D-RIGHT-GIMBAL--------------------------------------------------
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
pitch = data[1] * 256 + data[2];
roll = data[3] * 256 + data[4];
 
// Output data to serial monitor
//Serial.print("pitch: ");
//Serial.print(pitch);
//Serial.print("roll: ");
//Serial.println(roll);

//--------------------------------------------------LOOP-SCREEN--------------------------------------------------
  /*u8g.firstPage();  
    do {
      draw();
  } while( u8g.nextPage() );*/

//--------------------------------------------------LOOP--S-BUS-------------------------------------------------------
   /* uint32_t currentMillis = millis();
   
     //Here you can modify values of rcChannels while keeping it in 1000:2000 range
     
    if (currentMillis > sbusTime) {
        sbusPreparePacket(sbusPacket, rcChannels, false, false);
        Serial.write(sbusPacket, SBUS_PACKET_LENGTH);

        sbusTime = currentMillis + SBUS_UPDATE_RATE;
    }
*/
//--------------------------------------------------MENU--------------------------------------------------
int readingmenuok = digitalRead(inmenuok);
int readingmenuright = digitalRead(inmenuright);
int readingmenuleft = digitalRead(inmenuleft);
int readingmenuup = digitalRead(inmenuup);
int readingmenudown = digitalRead(inmenudown);

digitalWrite(inmenuok, HIGH);
digitalWrite(inmenuright, HIGH);
digitalWrite(inmenuleft, HIGH);
digitalWrite(inmenuup, HIGH);
digitalWrite(inmenudown, HIGH);

//--------------------------------------------------FILTRATION--------------------------------------------------
//Average Method 
/*
for(int i = 0; i < 200; ++i)
  {
    averagethrottle += Throttle;
    delay(1);
  }
  averagethrottle /= 200;
  */


//--------------------------------------------------CALIBRATION--------------------------------------------------
if (page == 1){   //Calibrate procedure begin
  //Serial.print(readingmenuok);
  //Serial.print("  ");
  
 if (readingmenuok == HIGH || calibrate == 1) {
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
//--------------------------------------------------EEPROM--------------------------------------------------
    //compact way to store calibration values in eeprom
    EEPROM.write(eepromaddrminthrottle, map(minthrottle, minhallsensorval, maxhallsensorval, 0, 255));    //EEPROM.write(Address, Value(from 0 to 255));
    EEPROM.write(eepromaddrmaxthrottle, map(maxthrottle, minhallsensorval, maxhallsensorval, 0, 255));
    EEPROM.write(eepromaddrtrimthrottle, map(trimthrottle, minhallsensorval, maxhallsensorval, 0, 255));
    
    EEPROM.write(eepromaddrminyaw, map(minyaw, minhallsensorval, maxhallsensorval, 0, 255));
    EEPROM.write(eepromaddrmaxyaw, map(maxyaw, minhallsensorval, maxhallsensorval, 0, 255));
    EEPROM.write(eepromaddrtrimyaw, map(trimyaw, minhallsensorval, maxhallsensorval, 0, 255));
    
    EEPROM.write(eepromaddrminpitch, map(minpitch, minhallsensorval, maxhallsensorval, 0, 255));
    EEPROM.write(eepromaddrmaxpitch, map(maxpitch, minhallsensorval, maxhallsensorval, 0, 255));
    EEPROM.write(eepromaddrtrimpitch, map(trimpitch, minhallsensorval, maxhallsensorval, 0, 255));
    
    EEPROM.write(eepromaddrminroll, map(minroll, minhallsensorval, maxhallsensorval, 0, 255));
    EEPROM.write(eepromaddrmaxroll, map(maxroll, minhallsensorval, maxhallsensorval, 0, 255)); 
    EEPROM.write(eepromaddrtrimroll, map(trimroll, minhallsensorval, maxhallsensorval, 0, 255));
    
  } //END Calibration
  
  //Serial.print("Calibration State:");
  //Serial.println(calibrate);
}

//reset calibrate procedure
if (readingmenuok == LOW){
    calibrate = 0;
}

//--------------------------------------------------TRIMS--------------------------------------------------



//---------------------------------------------------RC--------------------------------------------------
if (throttle <= trimthrottle) {    //------------------------------Throttle
  outputthrottle = map(throttle, minthrottle, trimthrottle, -100, 0);
}
else if (throttle > trimthrottle) {
  outputthrottle = map(throttle, trimthrottle, maxthrottle, 0, 100);
}

if (yaw <= trimyaw) {    //------------------------------yaw
  outputyaw = map(yaw, minyaw, trimyaw, -100, 0);
}
else if (yaw > trimyaw) {
  outputyaw = map(yaw, trimyaw, maxyaw, 0, 100);
}
outputyaw = - outputyaw; //reverse yaw output

if (pitch <= trimpitch) {    //------------------------------pitch
  outputpitch = map(pitch, minpitch, trimpitch, -100, 0);
}
else if (pitch > trimpitch) {
  outputpitch = map(pitch, trimpitch, maxpitch, 0, 100);
}

if (roll <= trimroll) {    //------------------------------roll
  outputroll = map(roll, minroll, trimroll, -100, 0);
}
else if (roll > trimroll) {
  outputroll = map(roll, trimroll, maxroll, 0, 100);
}
outputroll = - outputroll; //reverse roll output

//Constrain values
outputyaw = constrain(outputyaw, -100, 100);
outputthrottle = constrain(outputthrottle, -100, 100);
outputpitch = constrain(outputpitch, -100, 100);
outputroll = constrain(outputroll, -100, 100);

//--------------------------------------------------PPM--------------------------------------------------
//First step: Map output value for ppm
ppmthrottle = map(outputthrottle, -100, 100, 1000, 2000);
ppmyaw = map(outputyaw, -100, 100, 1000, 2000);
ppmpitch = map(outputpitch, -100, 100, 1000, 2000);
ppmroll = map(outputroll, -100, 100, 1000, 2000);

//Second step: Assign it to a ppm channel
ppm[1] = ppmthrottle;
ppm[2] = ppmpitch;
ppm[3] = ppmroll;
ppm[4] = ppmyaw;



//--------------------------------------------------NAVIGATION--------------------------------------------------
//Simple method
if (readingmenuright == LOW && page < MaxPage) { //menu right -> page+
  ++page;
  delay(50);
}

if (readingmenuleft == LOW && page > 0){ //menu left -> page-
  --page;
  delay(50);
}

//--------------------------------------------------Page-3-fan-control--------------------------------------------------
 voltage = analogRead(inputvoltage) * ( 5.00 / 1023.00) / 0.4443;
  Serial.print("Voltage:");
  Serial.println(voltage);

}
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------Void-loop-end----------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================

//--------------------------------------------------PPM-TO-LEAVE-ALONE--------------------------------------------------
/*ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if (state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PULSE_LENGTH * 2;
    state = false;
  } else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= CHANNEL_NUMBER){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;// 
      OCR1A = (FRAME_LENGTH - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PULSE_LENGTH) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
*/

//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-DRAW-PAGE---------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void draw(void) {
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(123, 57);
  u8g.print(page);

if (page == 0){ //--------------------------------------------------Page-0--------------------------------------------------
  u8g.drawBitmapP( 0, 0, 16, 64, screen1);
}

if (page == 1){ //--------------------------------------------------Page-1--------------------------------------------------
  
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

if (page == 2){ //--------------------------------------------------Page-2--------------------------------------------------

  //Draw Left Joystick
  xgraph1 = map(outputyaw, -100, 100, 8, 56);   
  ygraph1 = map(outputthrottle, 100, -100, 8, 56);
  u8g.drawCircle(32, 32, 20);
  u8g.drawCircle(xgraph1, ygraph1, 2);
  u8g.drawLine(32,32,xgraph1,ygraph1);
  
  //Draw Right Joystick
  xgraph2 = map(outputroll, -100, 100, 72, 120);  
  ygraph2 = map(outputpitch, 100, -100, 8, 56);
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


if (page == 3){ //--------------------------------------------------Page-3--------------------------------------------------

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

}
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-END---------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================




//===============================================================================================================================================================================================================
//---------------------------------------------------------------------------------------------------VOID-SBUS---------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
/*void sbusPreparePacket(uint8_t packet[], int channels[], bool isSignalLoss, bool isFailsafe){

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
//===============================================================================================================================================================================================================
//---------------------------------------------------------------------------------------------------END-VOID-SETUP----------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
