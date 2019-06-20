 /*TO-DO
 * 1- Make OTA timeout if no wifi instead of rebooting the ESP32 (OTA is now turned off)
 * 2- Soft Power
 * Turn off OTA via menu
 * Make navigation easier
 * Make calibration easier (Menu-> Calibrate -> press once, calibrate, press to stop calibration)
 */
const char* ssid = "HP-Nico";
const char* password = "nico1809";


#define RC 1 //SBus
//#define RC 2 //Mavlink //Need some work to add rc link & telemetry



#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "MLX.h"
#include "mavlink2\common\mavlink.h"


#include "Variables.h"

#include "OTA.h"  //Need some work to add timeout
#include "Battery.h"
#include "Buttons.h"
#include "ComputeRCData.h"
#include "EEPROM.h"
#include "Navigation.h" //Need some work to add highlights
#include "SH1106.h"
#include "SoftPower.h"  //Need some work 
#include "XBMP.h"

#if (RC == 1)
#include "SBus.h"
//#include "FrSkyX.h" //Need some work to add bind and rx num functionnality
#endif
#if (RC == 2)
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
  
  Wire.begin();   // Initialise I2C communication as MASTER
  Serial.begin(115200);  //Initialise Serial 1
  EEPROM.begin(12); //Initialise EEPROM and Ask for 12 addresses
  u8g2.begin(); //Initialise SH1106
  mlx.begin();  //Initialise MLX

  FirstBoot();
  OTASetup();
  PinModeDef();
  ReadEEPROM();  
  SBusInit();
  GetSketchName();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0);
  
}


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
  ReadVoltage();
  Screen();
  SoftPower();
  Serial.println(Step);

}
