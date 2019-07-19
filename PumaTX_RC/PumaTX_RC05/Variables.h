#ifndef Variables_h
#define Variables_h

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "MLX.h"


MLX mlx(0x0C, 0x0D);  //Left, Right
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

 
//--------------------------------------------------struct--------------------------------------------------
struct Channel {    
  int Reading;  //from -10000 to 10000
  int Min;  //MLX Scale
  int Max;  //MLX Scale
  int Trim; //MLX Scale
  int Output; //-100 to 100
  int PPM;
  const uint8_t EepromAddrMin;
  const uint8_t EepromAddrMax;
  const uint8_t EepromAddrTrim;
  uint8_t EepromMin;
  uint8_t EepromMax;
  uint8_t EepromTrim;
  bool reverse;
};

struct Button {
  uint8_t Pin;
  int State;
  int Output;
  int Prev;
  uint32_t Time;
};

struct ADC {
  uint8_t Pin;
  float State;
  int Process;
  int Output;
};

struct Graph {
  int x;
  int y;
};

Channel Throttle = {0, 0, 0, 0, 0, 992, 1, 2, 3, 0, 0, 0, 0};
Channel Yaw = {0, 0, 0, 0, 0, 992, 4, 5, 6, 0, 0, 0, 0};
Channel Pitch = {0, 0, 0, 0, 0, 992, 7, 8, 9, 0, 0, 0, 0};
Channel Roll = {0, 0, 0, 0, 0, 992, 10, 11, 12, 0, 0, 0, 0};

Button Right = {32, 1, 1, 1, 0};
Button Left = {33, 1, 1, 1, 0};
Button Up = {25, 1, 1, 1, 0};
Button Down = {26, 1, 1, 1, 0};
Button Ok = {27, 1, 1, 1, 0};
Button RTH = {14, 1, -100, 1, 0};
Button Pause = {13, 1, -100, 1, 0};
Button Pwr = {4, 1, 1, 1, 0};  //
Button Arm = {12, 1, -100, 1, 0};
Button Pre = {15, 1, -100, 1, 0};
Button LED = {2, 1, 1, 1, 0};
//Button C1 = {pin, 1, 1, 1, 0};
//Button C2 = {pin, 1, 1, 1, 0};

ADC Voltage = {A7, 0.00, 0, 0}; //GPIO35
ADC LeftPot = {A6, 0.0, 0, 0};  //GPIO34
ADC RightPot = {A3, 0.0, 0, 0}; //GPIO39

Graph LeftJoy = {0, 0};
Graph RightJoy = {0, 0};


//--------------------------------------------------VARIABLE--------------------------------------------------
bool calibrate = 0;

unsigned long lastdebouncetime = 0;
unsigned long debouncedelay = 50;
unsigned long currenttime = 0;

int page = 0;
int MaxPage = 5;

int HallSensorMin = -10000;
int HallSensorMax = 10000;

bool ScreenPwr;
unsigned long Now;

String Firmware;

int Step = 0;
int PowerProcedure = 0;

int BatteryPercentage = 0;

float lipo100 = 4.2;
float lipo90 = 4.13;
float lipo80 = 4.06;
float lipo70 = 3.99;
float lipo60 = 3.92;
float lipo50 = 3.85;
float lipo40 = 3.78;
float lipo30 = 3.71;
float lipo20 = 3.64;
float lipo10 = 3.57;
float lipo0 = 3.5;

int ok;

#endif
