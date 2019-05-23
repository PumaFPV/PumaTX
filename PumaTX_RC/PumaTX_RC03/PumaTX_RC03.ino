/*TO-DO
 * Make lib work
 * Make S-Bus work
 * Soft Power
 * Make calibration easier (Menu-> Calibrate -> press once, calibrate, press to stop calibration)
 * Add buttons for channel change
 */

const char* ssid = "iPhoneX";
const char* password = "nico1809";

#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "MLX.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SH1106.h"

MLX mlx(0x0C, 0x0D);  //Left, Right

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(21, 22);

//--------------------------------------------------S-BUS--------------------------------------------------
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

//--------------------------------------------------struct--------------------------------------------------
typedef struct Channel {    
  int Reading;  //from -10000 to 10000
  int Min;  //MLX Scale
  int Max;  //MLX Scale
  int Trim; //MLX Scale
  int Output; //-100 to 100
  int SBus;
  const uint8_t EepromAddrMin;
  const uint8_t EepromAddrMax;
  const uint8_t EepromAddrTrim;
  uint8_t EepromMin;
  uint8_t EepromMax;
  uint8_t EepromTrim;
  bool reverse;
};

typedef struct Button {
  uint8_t Pin;
  int State;
};

typedef struct ADC {
  uint8_t Pin;
  float State;
};

typedef struct Graph {
  int x;
  int y;
};

Channel Throttle = {0, 0, 0, 0, 0, 992, 1, 2, 3, 0, 0, 0, 0};
Channel Yaw = {0, 0, 0, 0, 0, 992, 4, 5, 6, 0, 0, 0, 0};
Channel Pitch = {0, 0, 0, 0, 0, 992, 7, 8, 9, 0, 0, 0, 0};
Channel Roll = {0, 0, 0, 0, 0, 992, 10, 11, 12, 0, 0, 0, 0};

Button Right = {32, 1};
Button Left = {33, 1};
Button Up = {25, 1};
Button Down = {26, 1};
Button Ok = {27, 1};
Button RTH = {14, 1};
Button Pause = {12, 1};
Button Power = {13, 1};
//Button Arm = {pin, 1};
//Button Pre = {pin, 1};
//Button C1 = {pin, 1};
//Button C2 = {pin, 1};

ADC Voltage = {A7, 0.00}; //GPIO35
ADC LeftPot = {A6, 0.0};  //GPIO34
ADC RightPot = {A3, 0.0}; //GPIO39

Graph Graph1 = {0, 0};
Graph Graph2 = {0, 0};


bool calibrate = 0;

unsigned long lastdebouncetime = 0;
unsigned long debouncedelay = 50;
unsigned long currenttime = 0;

int page = 0;
int MaxPage = 3;

int HallSensorMin = -10000;
int HallSensorMax = 10000;


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
  Serial.begin(115200);  
  EEPROM.begin(12); //Ask for 
  mlx.begin();
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(2000);
  display.clearDisplay();  
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

//--------------------------------------------------ShowSketchName--------------------------------------------------
    String path = __FILE__;
    int slash = path.lastIndexOf('\x5C');
    String the_cpp_name = path.substring(slash+1);
    int dot_loc = the_cpp_name.lastIndexOf('.');
    String Firmware = the_cpp_name.substring(0, dot_loc);
    Serial.println(Firmware);


//--------------------------------------------------SBUS--------------------------------------------------   
    for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        rcChannels[i] = 1500;
    }
    Serial2.begin(100000, SERIAL_8E2);  //SERIAL SBUS 
  

//--------------------------------------------------MENU--------------------------------------------------
  pinMode(Right.Pin, INPUT_PULLUP);
  pinMode(Left.Pin, INPUT_PULLUP);
  pinMode(Up.Pin, INPUT_PULLUP);
  pinMode(Down.Pin, INPUT_PULLUP);
  pinMode(Ok.Pin, INPUT_PULLUP);
  
//--------------------------------------------------READ-CALIBRATION-FROM-EEPROM--------------------------------------------------
//Enable if first time flash

/*
Serial.print(" Throttle:");
Serial.print(throttle);
Serial.print(" Min throttle:");
Serial.print(minthrottle);
Serial.print(" Max throttle:");
Serial.print(maxthrottle);
Serial.print(" Trim throttle:");
Serial.println(trimthrottle);


    EEPROM.write(eepromaddrminthrottle, 250);    //EEPROM.write(Address, Value(from 0 to 255));
    EEPROM.write(eepromaddrmaxthrottle, 10);
    EEPROM.write(eepromaddrtrimthrottle, 124);
    
    EEPROM.write(eepromaddrminyaw, 250);
    EEPROM.write(eepromaddrmaxyaw, 10);
    EEPROM.write(eepromaddrtrimyaw, 124);
    
    EEPROM.write(eepromaddrminpitch, 250);
    EEPROM.write(eepromaddrmaxpitch, 10);
    EEPROM.write(eepromaddrtrimpitch, 124);
    
    EEPROM.write(eepromaddrminroll, 250);
    EEPROM.write(eepromaddrmaxroll, 10); 
    EEPROM.write(eepromaddrtrimroll, 124);

    EEPROM.commit();

*/

  Throttle.Min = map(EEPROM.read(Throttle.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Throttle.Max = map(EEPROM.read(Throttle.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Throttle.Trim = map(EEPROM.read(Throttle.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);
  Yaw.Min = map(EEPROM.read(Yaw.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Yaw.Max = map(EEPROM.read(Yaw.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Yaw.Trim = map(EEPROM.read(Yaw.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);
  Pitch.Min = map(EEPROM.read(Pitch.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Pitch.Max = map(EEPROM.read(Pitch.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Pitch.Trim = map(EEPROM.read(Pitch.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);
  Roll.Min = map(EEPROM.read(Roll.EepromAddrMin), 0, 255, HallSensorMin, HallSensorMax);
  Roll.Max = map(EEPROM.read(Roll.EepromAddrMax), 0, 255, HallSensorMin, HallSensorMax);
  Roll.Trim = map(EEPROM.read(Roll.EepromAddrTrim), 0, 255, HallSensorMin, HallSensorMax);  

//--------------------------------------------------OTA--------------------------------------------------
WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    ESP.restart();
  }
  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
    })
    .onEnd([]() {
    })
    .onProgress([](unsigned int progress, unsigned int total) {})
    .onError([](ota_error_t error) {
      if (error == OTA_AUTH_ERROR);
      else if (error == OTA_BEGIN_ERROR);
      else if (error == OTA_CONNECT_ERROR);
      else if (error == OTA_RECEIVE_ERROR);
      else if (error == OTA_END_ERROR);
    });

  ArduinoOTA.begin();

//--------------------------------------------------OTA-END--------------------------------------------------


}
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------END-VOID-SETUP---------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================








//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-LOOP--------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void loop(){
  ArduinoOTA.handle();
  mlx.process();

//--------------------------------------------------LOOP-MLX--------------------------------------------------
Throttle.Reading = mlx.getThrottle();
Yaw.Reading = mlx.getYaw();
Pitch.Reading = mlx.getPitch();
Roll.Reading = mlx.getRoll();

//--------------------------------------------------LOOP-SCREEN--------------------------------------------------
 /* u8g.firstPage();  
    do {
      draw();
  } while( u8g.nextPage() );
  */
  //draw();
  drawscreen();
  display.display();

//--------------------------------------------------LOOP--S-BUS-------------------------------------------------------
uint32_t currentMillis = millis();

    if (currentMillis > sbusTime) {
        sbusPreparePacket(sbusPacket, rcChannels, false, false);
        Serial.write(sbusPacket, SBUS_PACKET_LENGTH);

        sbusTime = currentMillis + SBUS_UPDATE_RATE;
    }

//--------------------------------------------------MENU--------------------------------------------------
    Ok.State = digitalRead(Ok.Pin);
    Right.State = digitalRead(Right.Pin);
    Left.State = digitalRead(Left.Pin);
    Up.State = digitalRead(Up.Pin);
    Down.State = digitalRead(Down.Pin);

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
  
 if (Ok.State == 1 || calibrate == 1) {
    lastdebouncetime = millis();
 }
  if (millis() - lastdebouncetime > 2000){
    calibrate = 1;

    //Calibrate trims
    Throttle.Trim = Throttle.Reading;  
    Yaw.Trim = Yaw.Reading;
    Pitch.Trim = Pitch.Reading;
    Roll.Trim = Roll.Reading;  

    //Calibrate min/max----------Throttle
    if (Throttle.Reading > Throttle.Max){
      Throttle.Max = Throttle.Reading - 20;
    }
    if(Throttle.Reading < Throttle.Min){
      Throttle.Min = Throttle.Reading + 20;
    }
    
    //Calibrate min/max----------Yaw
    if (Yaw.Reading > Yaw.Max){
      Yaw.Max = Yaw.Reading - 20;
    }
    if (Yaw.Reading < Yaw.Min){
      Yaw.Min = Yaw.Reading + 20;
    }

    //Calibrate min/max----------Pitch
    if (Pitch.Reading > Pitch.Max){
      Pitch.Max = Pitch.Reading - 20;
    }
    if (Pitch.Reading < Pitch.Min){
      Pitch.Min = Pitch.Reading + 20;
    }

    //Calibrate min/max----------Roll
    if (Roll.Reading > Roll.Max){
      Roll.Max = Roll.Reading - 20;
    }
    if (Roll.Reading < Roll.Min){
      Roll.Min = Roll.Reading + 20;
    }
//--------------------------------------------------EEPROM--------------------------------------------------
    //compact way to store calibration values in eeprom
    EEPROM.write(Throttle.EepromAddrMin, map(Throttle.Min, HallSensorMin, HallSensorMax, 0, 255));    //EEPROM.write(Address, Value(from 0 to 255));
    EEPROM.write(Throttle.EepromAddrMax, map(Throttle.Max, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Throttle.EepromAddrTrim, map(Throttle.Trim, HallSensorMin, HallSensorMax, 0, 255));
    
    EEPROM.write(Yaw.EepromAddrMin, map(Yaw.Min, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Yaw.EepromAddrMax, map(Yaw.Max, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Yaw.EepromAddrTrim, map(Yaw.Trim, HallSensorMin, HallSensorMax, 0, 255));
    
    EEPROM.write(Pitch.EepromAddrMin, map(Pitch.Min, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Pitch.EepromAddrMax, map(Pitch.Max, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Pitch.EepromAddrTrim, map(Pitch.Trim, HallSensorMin, HallSensorMax, 0, 255));
    
    EEPROM.write(Roll.EepromAddrMin, map(Roll.Min, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.write(Roll.EepromAddrMax, map(Roll.Max, HallSensorMin, HallSensorMax, 0, 255)); 
    EEPROM.write(Roll.EepromAddrTrim, map(Roll.Trim, HallSensorMin, HallSensorMax, 0, 255));
    EEPROM.commit();
  } //END Calibration
  
  //Serial.print("Calibration State:");
  //Serial.println(calibrate);
}
else 
calibrate = 0;

//reset calibrate procedure

/* if (OK.State == 0){
    calibrate = 0;
}
*/
//--------------------------------------------------TRIMS--------------------------------------------------



//---------------------------------------------------RC--------------------------------------------------
if (Throttle.Reading <= Throttle.Trim) {    //------------------------------Throttle
  Throttle.Output = map(Throttle.Reading, Throttle.Min, Throttle.Trim, -100, 0);
}
else if (Throttle.Reading > Throttle.Trim) {
  Throttle.Output = map(Throttle.Reading, Throttle.Trim, Throttle.Max, 0, 100);
}

if (Yaw.Reading <= Yaw.Trim) {    //------------------------------yaw
  Yaw.Output = map(Yaw.Reading, Yaw.Min, Yaw.Trim, -100, 0);
}
else if (Yaw.Reading > Yaw.Trim) {
  Yaw.Output = map(Yaw.Reading, Yaw.Trim, Yaw.Max, 0, 100);
}
Yaw.Output = - Yaw.Output; //reverse yaw output

if (Pitch.Reading <= Pitch.Trim) {    //------------------------------pitch
  Pitch.Output = map(Pitch.Reading, Pitch.Min, Pitch.Trim, -100, 0);
}
else if (Pitch.Reading > Pitch.Trim) {
  Pitch.Output = map(Pitch.Reading, Pitch.Trim, Pitch.Max, 0, 100);
}

if (Roll.Reading <= Roll.Trim) {    //------------------------------roll
  Roll.Output = map(Roll.Reading, Roll.Min, Roll.Trim, -100, 0);
}
else if (Roll.Reading > Roll.Trim) {
  Roll.Output = map(Roll.Reading, Roll.Trim, Roll.Max, 0, 100);
}
  Roll.Output = - Roll.Output; //reverse roll output

//Constrain values
Throttle.Output = constrain(Throttle.Output, -100, 100);
Yaw.Output = constrain(Yaw.Output, -100, 100);
Roll.Output = constrain(Roll.Output, -100, 100);


//--------------------------------------------------NAVIGATION--------------------------------------------------
//Simple method
if (Right.State == 0 && page < MaxPage) { //menu right -> page+
  ++page;
  delay(50);
}

if (Left.State == 0 && page > 0){ //menu left -> page-
  --page;
  delay(50);
}


//--------------------------------------------------Page-3-fan-control--------------------------------------------------
  Voltage.State = analogRead(Voltage.Pin) * ( 5.00 / 1023.00) / 0.4443;
  /*
  Serial.print("Voltage:");
  Serial.println(voltage);
  */
}
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------Void-loop-end----------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================



//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-DRAW-PAGE---------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
 
void drawscreen(void) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0); //123,57
  display.print(page);
  display.display();
  }
/*
if (page == 0){ //--------------------------------------------------Page-0--------------------------------------------------
  u8g.drawBitmapP( 0, 0, 16, 64, screen1);
}

if (page == 1){ //--------------------------------------------------Page-1--------------------------------------------------
  
  //Throttle
  u8g.setPrintPos(0, 8); 
  u8g.print("Throttle:");
  u8g.setPrintPos(0, 16);
  u8g.print(Throttle.MLX);
  u8g.setPrintPos(0, 24);
  u8g.print(Throttle.Output);
  
  //Yaw
  u8g.setPrintPos(0, 40); 
  u8g.print("Yaw:");
  u8g.setPrintPos(0, 48);
  u8g.print(Yaw.MLX);
  u8g.setPrintPos(0, 56);
  u8g.print(Yaw.Output);
  
  //Pitch
  u8g.setPrintPos(64, 8); 
  u8g.print("Pitch:");
  u8g.setPrintPos(64, 16);
  u8g.print(Pitch.MLX);
  u8g.setPrintPos(64, 24);
  u8g.print(Pitch.Output);
  
  //Roll
  u8g.setPrintPos(64, 40); 
  u8g.print("Roll:");
  u8g.setPrintPos(64, 48);
  u8g.print(Roll.MLX);
  u8g.setPrintPos(64, 56);
  u8g.print(Roll.Output);

}

if (page == 2){ //--------------------------------------------------Page-2--------------------------------------------------

  //Draw Left Joystick
  LeftJoy.x = map(Yaw.Output, -100, 100, 8, 56);   
  LeftJoy.y = map(Throttle.Output, 100, -100, 8, 56);
  u8g.drawCircle(32, 32, 20);
  u8g.drawCircle(LeftJoy.x, LeftJoy.y, 2);
  u8g.drawLine(32, 32, LeftJoy.x, LeftJoy.y);
  
  //Draw Right Joystick
  RightJoy.x = map(Roll.Output, -100, 100, 72, 120);  
  RightJoy.y = map(Pitch.Output, 100, -100, 8, 56);
  u8g.drawCircle(96, 32, 20);
  u8g.drawCircle(RightJoy.x, RightJoy.y, 2);
  u8g.drawLine(96, 32, RightJoy.x, RightJoy.y);

  //Serial.print(xgraph1);
  //Serial.print(" ");
  //Serial.print(ygraph1);
  //Serial.print(" ");
  //Serial.print(xgraph2);
  //Serial.print(" ");
  //Serial.println(ygraph2);
   
  
}


if (page == 3){ //--------------------------------------------------Page-3--------------------------------------------------

  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 11);
  u8g.print("Setup");

  //Voltage
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(0, 50);
  u8g.print("Voltage:");
  u8g.setPrintPos(0, 57);
  u8g.print(Voltage.State);

}

}
*/
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-END---------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================




//===============================================================================================================================================================================================================
//---------------------------------------------------------------------------------------------------VOID-SBUS---------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
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


//===============================================================================================================================================================================================================
//---------------------------------------------------------------------------------------------------END-VOID-SETUP----------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
