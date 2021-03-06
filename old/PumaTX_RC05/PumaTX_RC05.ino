const char* ssid = "HP-Nico";
const char* password = "Nico1809";

#define RC 1 //SBus
//#define RC 2 //Mavlink //Need some work to add rc link & telemetry

//#define LINK 1  //OTA
#define LINK 0  //Bluetooth Serial


#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "MLX.h"

#if (LINK == 1)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "OTA.h"  //Need some work to add timeout
#endif
#if (LINK == 2)
#include "BluetoothSerial.h"
BluetoothSerial SerialBT; 
#endif


#include "Variables.h"

#include "Battery.h"
#include "Buttons.h"
#include "ComputeRCData.h"
#include "EEPROM.h"
#include "Navigation.h" //Need some work to add highlights
#include "Screen.h"
#include "SoftPower.h"  //Need some work 
#include "XBMP.h"

#if (RC == 1)
#include "SBus.h"
//#include "FrSkyX.h" //Need some work to add bind and rx num functionnality
#endif
#if (RC == 2)
//#include "Libraries\mavlink2\common\mavlink.h"
#include "Mavlink2.h"
#endif

//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void GetSketchName(){
  
    String path = __FILE__;
    int slash = path.lastIndexOf('\x5C');
    String the_cpp_name = path.substring(slash+1);
    int dot_loc = the_cpp_name.lastIndexOf('.');
    Firmware = the_cpp_name.substring(0, dot_loc);
    //Serial.println(Firmware);
    
}

void setup(void){
  if(LINK == 2){
    //SerialBT.begin("PumaTX"); //Bluetooth device name
  }

  Wire.begin();   // Initialise I2C communication as MASTER
  Serial.begin(115200);  //Initialise Serial 1
  EEPROM.begin(12); //Initialise EEPROM and Ask for 12 addresses
  u8g2.begin(); //Initialise SH1106
  mlx.begin();  //Initialise MLX

  SoftPowerInit();
  FirstBoot();
  
  if(LINK == 1){
    //OTASetup();
  }
  
  PinModeDef();
  ReadEEPROM();  
  GetSketchName();
  
  if (RC == 1){
    SBusInit();
  }
  if (RC == 2){
    //MavlinkSetup();
  }
  
}


void loop(void){
  
  if(LINK == 1){
    //ArduinoOTA.handle();
  }
  mlx.process();
  GetMLXData();
  Calibrate();
  //ComputeRC();
  ComputeRC3();
  ProcessButtons();
  
  if (RC == 1){
    SBus();
  }
  
  Navigation();
  ScreenLoop();
  
  /*  SerialBT.print((String)"State:" + Pause.State);
  SerialBT.print("    ");
  SerialBT.print((String)"Step:" + Step);
  SerialBT.print("     ");
  SerialBT.println((String)"Time:" + (millis() - Pwr.Time));  
*/ 
}
