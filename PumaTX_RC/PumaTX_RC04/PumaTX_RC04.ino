/*TO-DO
 * 1- Make OTA timeout if no wifi instead of rebooting the ESP32 (OTA is now turned off)
 * 2- Add support for onboard buttons
 * 3- Soft Power
 * 4- Add buttons for channel change
 * Turn off OTA via menu
 * Make navigation easier
 * Make calibration easier (Menu-> Calibrate -> press once, calibrate, press to stop calibration)

 */

const char* ssid = "NICOLASDG";
const char* password = "Nico1809";



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
//--------------------------------------------------S-BUS--------------------------------------------------
#define RC_CHANNEL_MIN -100  
#define RC_CHANNEL_MAX 100   

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
  int Output;
  int Prev;
};

typedef struct ADC {
  uint8_t Pin;
  float State;
  int Output;
};

typedef struct Graph {
  int x;
  int y;
};

Channel Throttle = {0, 0, 0, 0, 0, 992, 1, 2, 3, 0, 0, 0, 0};
Channel Yaw = {0, 0, 0, 0, 0, 992, 4, 5, 6, 0, 0, 0, 0};
Channel Pitch = {0, 0, 0, 0, 0, 992, 7, 8, 9, 0, 0, 0, 0};
Channel Roll = {0, 0, 0, 0, 0, 992, 10, 11, 12, 0, 0, 0, 0};

Button Right = {32, 1, 1, 1};
Button Left = {33, 1, 1, 1};
Button Up = {25, 1, 1, 1};
Button Down = {26, 1, 1, 1};
Button Ok = {27, 1, 1, 1};
Button RTH = {14, 1, -100, 1};
Button Pause = {13, 1, -100, 1};
Button Pwr = {4, 1, 1, 1};
Button Arm = {12, 1, -100, 1};
Button Pre = {15, 1, -100, 1};
Button LED = {2, 1, 1, 1};
//Button C1 = {pin, 1, 1, 1};
//Button C2 = {pin, 1, 1, 1};

ADC Voltage = {A7, 0.00, 0}; //GPIO35
ADC LeftPot = {A6, 0.0, 0};  //GPIO34
ADC RightPot = {A3, 0.0, 0}; //GPIO39

Graph LeftJoy = {0, 0};
Graph RightJoy = {0, 0};


//--------------------------------------------------VARIABLE--------------------------------------------------
bool calibrate = 0;

unsigned long lastdebouncetime = 0;
unsigned long debouncedelay = 50;
unsigned long currenttime = 0;

int page = 0;
int MaxPage = 4;

int HallSensorMin = -10000;
int HallSensorMax = 10000;

bool ScreenPwr;
unsigned long Now;

String Firmware;


//--------------------------------------------------BITMAP--------------------------------------------------

static const unsigned char Puma[] PROGMEM = {
  0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0xF0, 0x06, 0x00, 0x00, 
  0x00, 0x00, 0xFC, 0x01, 0xF8, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x66, 0x03, 
  0x68, 0x31, 0x00, 0x00, 0x00, 0x80, 0xD3, 0x06, 0x6C, 0x63, 0x00, 0x00, 
  0x00, 0xC0, 0xD8, 0x06, 0x24, 0xC6, 0x00, 0xF8, 0x03, 0x60, 0x8C, 0x04, 
  0x26, 0x0C, 0xE3, 0xFF, 0xFF, 0x19, 0x84, 0x0C, 0x36, 0x08, 0xFE, 0x40, 
  0xE0, 0x0F, 0x82, 0x0D, 0x32, 0xD0, 0x1D, 0x40, 0x00, 0x77, 0x83, 0x09, 
  0x12, 0x70, 0x7C, 0x40, 0xC0, 0xCF, 0x81, 0x09, 0x06, 0x3E, 0xCE, 0x40, 
  0x60, 0x8E, 0x8F, 0x0D, 0x9C, 0x1B, 0x9B, 0x41, 0x30, 0x9B, 0x39, 0x07, 
  0xF8, 0x18, 0x39, 0x43, 0x98, 0x9B, 0xE3, 0x03, 0x30, 0x2C, 0x3D, 0x4C, 
  0x86, 0x92, 0x06, 0x03, 0x10, 0xAC, 0x29, 0x58, 0x83, 0xB2, 0x04, 0x01, 
  0x30, 0xA6, 0x68, 0xE0, 0xC1, 0xA2, 0x8C, 0x01, 0x30, 0xE6, 0x68, 0xF8, 
  0xC3, 0xE2, 0x8C, 0x01, 0x70, 0x62, 0xC8, 0x5F, 0x7E, 0xC2, 0xC8, 0x00, 
  0x70, 0x63, 0xC8, 0x40, 0x60, 0xC2, 0xD8, 0x00, 0x70, 0x71, 0xC8, 0x40, 
  0x40, 0xC0, 0xD3, 0x01, 0xE0, 0x5D, 0x68, 0x40, 0xE0, 0x42, 0xF6, 0x00, 
  0xC0, 0x43, 0xA8, 0x40, 0xA0, 0x62, 0x78, 0x00, 0xC0, 0xC0, 0xB0, 0x41, 
  0xA0, 0x63, 0x60, 0x00, 0xC0, 0x80, 0xB0, 0x41, 0xB0, 0x23, 0x60, 0x00, 
  0x40, 0x80, 0x99, 0x41, 0x30, 0x33, 0x40, 0x00, 0x40, 0xFE, 0x1B, 0x41, 
  0x10, 0xFB, 0x4F, 0x00, 0xC0, 0xC3, 0x18, 0x41, 0x10, 0x63, 0x7C, 0x00, 
  0xC0, 0x40, 0x30, 0x41, 0x90, 0x41, 0x40, 0x00, 0xC0, 0x60, 0x30, 0x40, 
  0x98, 0xC1, 0x60, 0x00, 0xC0, 0xE0, 0x2F, 0x43, 0x98, 0xFE, 0x60, 0x00, 
  0xC0, 0xB1, 0x7F, 0x42, 0xD8, 0xBF, 0x61, 0x00, 0xC0, 0xB9, 0xF9, 0x46, 
  0xCC, 0x93, 0x73, 0x00, 0xC0, 0xB7, 0xC7, 0xFE, 0x6F, 0xBC, 0x7F, 0x00, 
  0x80, 0xB3, 0xE7, 0xF3, 0xF9, 0xBC, 0x39, 0x00, 0x80, 0x73, 0xBC, 0xC1, 
  0xF8, 0xC7, 0x19, 0x00, 0x80, 0x67, 0x8C, 0xA1, 0x30, 0x86, 0x3D, 0x00, 
  0x80, 0x2D, 0x0C, 0xB1, 0x11, 0x86, 0x35, 0x00, 0x00, 0x19, 0x98, 0x19, 
  0x33, 0x8F, 0x33, 0x00, 0x00, 0x3B, 0x96, 0x09, 0x22, 0x8D, 0x1B, 0x00, 
  0x00, 0x32, 0xB6, 0x0C, 0x66, 0x8D, 0x09, 0x00, 0x00, 0x66, 0xF6, 0x04, 
  0xC4, 0xC9, 0x0C, 0x00, 0x00, 0x6C, 0x62, 0x06, 0xC8, 0xC8, 0x06, 0x00, 
  0x00, 0xD8, 0x63, 0x03, 0xF8, 0x78, 0x03, 0x00, 0x00, 0x10, 0x33, 0x01, 
  0xB0, 0x19, 0x01, 0x00, 0x00, 0x30, 0x13, 0x01, 0x10, 0x9B, 0x01, 0x00, 
  0x00, 0xC0, 0x8B, 0x01, 0x30, 0xFA, 0x00, 0x00, 0x00, 0xC0, 0x8E, 0x01, 
  0x30, 0x6E, 0x00, 0x00, 0x00, 0x80, 0x86, 0x01, 0x30, 0x2C, 0x00, 0x00, 
  0x00, 0x80, 0x87, 0x05, 0x30, 0x38, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 
  0x1F, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF3, 0x19, 0x18, 0x00, 0x00, 
  0x00, 0x00, 0xC3, 0x0E, 0x7C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x1C, 
  0xC6, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x18, 0x03, 0x0E, 0x00, 0x00, 
  0x00, 0x00, 0x04, 0xF0, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40, 
  0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x18, 0xE0, 0x00, 0x03, 0x00, 0x00, 
  0x00, 0x00, 0x70, 0xFC, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x1F, 
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x18, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};




//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void setup(void){
  
  Wire.begin();   // Initialise I2C communication as MASTER
  Serial.begin(115200);  
  EEPROM.begin(12); //Ask for 12 addresses
  u8g2.begin();
  mlx.begin();

  ShowSketchName();
  SBusInit();
  ReadEEPROM();
  PinModeDef();
  FirstBoot();
  OTASetup();
  
}
//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------END-VOID-SETUP---------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================


//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-LOOP--------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void loop(void){
  ArduinoOTA.handle();
  mlx.process();
  GetMLXData();
  Calibrate();
  //ComputeRC();
  ComputeRC2();
  ProcessButtons();
  SBus();
  Navigation();
  //OptimizeScreenUsage();
  Screen();
  ReadVoltage();

}

//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------Void-loop-end----------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void ProcessButtons(){
  RightPot.State = analogRead(RightPot.Pin);
  LeftPot.State = analogRead(LeftPot.Pin);
  Arm.State = digitalRead(Arm.Pin);
  Pre.State = digitalRead(Pre.Pin);
  RTH.State = digitalRead(RTH.Pin);
  Pwr.State = digitalRead(Pwr.Pin);
}

void ComputeRC2(){
  if(Throttle.Reading < 35){
    Throttle.Output = map(Throttle.Reading, 0, 34, 0, 100);
  }
  else 
  Throttle.Output = map(Throttle.Reading, 255, 223, -1, -100);

  if(Yaw.Reading < 38){
    Yaw.Output = map(Yaw.Reading, 37, 5, -100, 0);
  }
  else 
  Yaw.Output = map(Yaw.Reading, 255, 224, 0, 100);
  
  if(Pitch.Reading < 36){
    Pitch.Output = map(Pitch.Reading, 4, 35, 0, 100);
  }
  else 
  Pitch.Output = map(Pitch.Reading, 255, 224, -1, -100);  //May need to be adjusted
  
  if(Roll.Reading < 36){
    Roll.Output = map(Roll.Reading, 0, 35, -1, -100);
  }
  else 
  Roll.Output = map(Roll.Reading, 255, 221, 1, 100);

  RightPot.Output = map(RightPot.State, 0, 4095, -100, 100);
  constrain(RightPot.Output, -100, 100);

  LeftPot.Output = map(LeftPot.State, 3570, 440, -100, 100);
  constrain(LeftPot.Output, -100, 100);

  if (Arm.State == 0 && Arm.Prev == 1 && millis() - currenttime > debouncedelay) {
    if (Arm.Output == -100)
      Arm.Output = 100;
    else
      Arm.Output = -100;

    currenttime = millis();    
  }

  Arm.Prev = Arm.State;

  if (RTH.State == 0 && RTH.Prev == 1 && millis() - currenttime > debouncedelay) {
    if (RTH.Output == -100)
      RTH.Output = 100;
    else
      RTH.Output = -100;

    currenttime = millis();    
  }

  Arm.Prev = Arm.State;

  Pre.Output = map(Pre.State, 0, 1, 100, -100);
  
}

void FirstBoot(){
    EEPROM.write(Throttle.EepromAddrMin, 250);    //EEPROM.write(Address, Value(from 0 to 255));
    EEPROM.write(Throttle.EepromAddrMax, 10);
    EEPROM.write(Throttle.EepromAddrTrim, 124);
    
    EEPROM.write(Yaw.EepromAddrMin, 250);
    EEPROM.write(Yaw.EepromAddrMin, 10);
    EEPROM.write(Yaw.EepromAddrTrim, 124);
    
    EEPROM.write(Pitch.EepromAddrMin, 250);
    EEPROM.write(Pitch.EepromAddrMax, 10);
    EEPROM.write(Pitch.EepromAddrTrim, 124);
    
    EEPROM.write(Roll.EepromAddrMin, 250);
    EEPROM.write(Roll.EepromAddrMax, 10); 
    EEPROM.write(Roll.EepromAddrTrim, 124);

    EEPROM.commit();
}

void PinModeDef(){
  pinMode(Right.Pin, INPUT_PULLUP);
  pinMode(Left.Pin, INPUT_PULLUP);
  pinMode(Up.Pin, INPUT_PULLUP);
  pinMode(Down.Pin, INPUT_PULLUP);
  pinMode(Ok.Pin, INPUT_PULLUP);    
  pinMode(LED.Pin, OUTPUT); 
  pinMode(Arm.Pin, INPUT_PULLUP);
  pinMode(Pre.Pin, INPUT_PULLUP);
  pinMode(RTH.Pin, INPUT_PULLUP);
  digitalWrite(LED.Pin, Pwr.State);
}

void SBusInit(){
   for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        rcChannels[i] = 1500;
    }
    Serial2.begin(100000, SERIAL_8E2);  //SERIAL SBUS 
}

void ReadEEPROM(){
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
}


void OTASetup(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    ESP.restart();
  }
  WiFi.setHostname("PumaTX");
  
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
}


void ShowSketchName(){
    String path = __FILE__;
    int slash = path.lastIndexOf('\x5C');
    String the_cpp_name = path.substring(slash+1);
    int dot_loc = the_cpp_name.lastIndexOf('.');
    Firmware = the_cpp_name.substring(0, dot_loc);
    //Serial.println(Firmware);
}

void GetMLXData(){
  Throttle.Reading = mlx.getThrottle();
  Yaw.Reading = mlx.getYaw();
  Pitch.Reading = mlx.getPitch();
  Roll.Reading = mlx.getRoll();
}

void SBus(){

  rcChannels[0] = Throttle.Output;
  rcChannels[1] = Pitch.Output;
  rcChannels[2] = Roll.Output;
  rcChannels[3] = Yaw.Output;
  rcChannels[4] = Arm.Output;
  rcChannels[5] = Pre.Output;    
  rcChannels[6] = RightPot.Output;
  rcChannels[7] = LeftPot.Output; 
  rcChannels[8] = RTH.Output;
  
  uint32_t currentMillis = millis();

    if (currentMillis > sbusTime) {
      
        sbusPreparePacket(sbusPacket, rcChannels, false, false);
        Serial2.write(sbusPacket, SBUS_PACKET_LENGTH);

        sbusTime = currentMillis + SBUS_UPDATE_RATE;
    }
}

void OptimizeScreenUsage(){
  if (ScreenPwr == 1 && page == 0){
    delay(500);
    ScreenPwr = 0;
    //Serial.println(ScreenPwr);
  }

  if (page == 1 && ScreenPwr == 0){
    ScreenPwr = 1;
  }

  if (ScreenPwr == 1){
    Screen();
  }
  
}

void Screen(){
    u8g2.firstPage();  
    do {
      DrawScreen();
    } while ( u8g2.nextPage() );
}

void ReadVoltage(){
  Voltage.State = analogRead(Voltage.Pin) * ( 5.00 / 1023.00) / 0.4443;
}

void Navigation(){
  
    Ok.State = digitalRead(Ok.Pin);
    Right.State = digitalRead(Right.Pin);
    Left.State = digitalRead(Left.Pin);
    Up.State = digitalRead(Up.Pin);
    Down.State = digitalRead(Down.Pin);
    
  if (Right.State == 0 && page < MaxPage) { //menu right -> page+
    ++page;
    delay(50);
  }

  if (Left.State == 0 && page > 0){ //menu left -> page-
    --page;
    delay(50);
  }
}

void Calibrate(){
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

}


void ComputeRC(){
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

  Throttle.Output = constrain(Throttle.Output, -100, 100);
  Yaw.Output = constrain(Yaw.Output, -100, 100);
  Pitch.Output = constrain(Pitch.Output, -100, 100);
  Roll.Output = constrain(Roll.Output, -100, 100);

}


//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-DRAW-PAGE---------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
 
void DrawScreen(void) {
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.setCursor(123, 57);
  u8g2.print(page);


if (page == 0){ //--------------------------------------------------Page-0--------------------------------------------------
  u8g2.drawXBMP( 0, 0, 60, 64, Puma);
  
  u8g2.setFont(u8g2_font_fub17_tr);
  u8g2.setCursor(63, 27);
  u8g2.print("Puma");
  u8g2.setFont(u8g2_font_fub25_tr);
  u8g2.setCursor(64, 54);
  u8g2.print("TX");
  
}

if (page == 1){ //--------------------------------------------------Page-1--------------------------------------------------
  
  //Throttle
  u8g2.setCursor(0, 8); 
  u8g2.print("Throttle:");
  u8g2.setCursor(0, 16);
  u8g2.print(Throttle.Reading);
  u8g2.setCursor(0, 24);
  u8g2.print(Throttle.Output);
  
  //Yaw
  u8g2.setCursor(0, 32); 
  u8g2.print("Yaw:");
  u8g2.setCursor(0, 40);
  u8g2.print(Yaw.Reading);
  u8g2.setCursor(0, 48);
  u8g2.print(Yaw.Output);
  
  //Pitch
  u8g2.setCursor(64, 8); 
  u8g2.print("Pitch:");
  u8g2.setCursor(64, 16);
  u8g2.print(Pitch.Reading);
  u8g2.setCursor(64, 24);
  u8g2.print(Pitch.Output);
  
  //Roll
  u8g2.setCursor(64, 32); 
  u8g2.print("Roll:");
  u8g2.setCursor(64, 40);
  u8g2.print(Roll.Reading);
  u8g2.setCursor(64, 48);
  u8g2.print(Roll.Output);

  u8g2.setCursor(0, 56); 
  u8g2.print(LeftPot.Output);
  u8g2.setCursor(0, 64);
  u8g2.print(Arm.Output);


  u8g2.setCursor(64, 56); 
  u8g2.print(RightPot.Output);
  u8g2.setCursor(64, 64);
  u8g2.print(Pre.Output);  

}

if (page == 2){ //--------------------------------------------------Page-2--------------------------------------------------

  //Draw Left Joystick
  LeftJoy.x = map(Yaw.Output, -100, 100, 8, 56);   
  LeftJoy.y = map(Throttle.Output, 100, -100, 8, 56);
  u8g2.drawCircle(32, 32, 20);
  u8g2.drawCircle(LeftJoy.x, LeftJoy.y, 2);
  u8g2.drawLine(32, 32, LeftJoy.x, LeftJoy.y);
  
  //Draw Right Joystick
  RightJoy.x = map(Roll.Output, -100, 100, 72, 120);  
  RightJoy.y = map(Pitch.Output, 100, -100, 8, 56);
  u8g2.drawCircle(96, 32, 20);
  u8g2.drawCircle(RightJoy.x, RightJoy.y, 2);
  u8g2.drawLine(96, 32, RightJoy.x, RightJoy.y);

  //Serial.print(xgraph1);
  //Serial.print(" ");
  //Serial.print(ygraph1);
  //Serial.print(" ");
  //Serial.print(xgraph2);
  //Serial.print(" ");
  //Serial.println(ygraph2);
   
  
}


if (page == 3){ //--------------------------------------------------Page-3--------------------------------------------------

  u8g2.setFont(u8g_font_unifont);
  u8g2.setCursor(0, 11);
  u8g2.print("Setup");
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 20);
  u8g2.print(Firmware);
  
  //Voltage
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 50);
  u8g2.print("Voltage:");
  u8g2.setCursor(0, 57);
  u8g2.print(Voltage.State);

  }


if (page == 4){ //--------------------------------------------------Page-4--------------------------------------------------

  u8g2.setFont(u8g_font_unifont);
  u8g2.setCursor(0, 11);
  u8g2.print("Config");
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 20);
  u8g2.print("OTA:");
  u8g2.setCursor(25, 20);
  u8g2.print("OTA.State");
  u8g2.setCursor(0, 27);
  u8g2.print("");
  
  }
}

//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------VOID-END-SCREEN--------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------END-VOID-SBus-----------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
