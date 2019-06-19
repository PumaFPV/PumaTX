 /*TO-DO
 * 1- Make OTA timeout if no wifi instead of rebooting the ESP32 (OTA is now turned off)
 * 2- Soft Power
 * Turn off OTA via menu
 * Make navigation easier
 * Make calibration easier (Menu-> Calibrate -> press once, calibrate, press to stop calibration)
 */

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

MLX mlx(0x0C, 0x0D);  //Left, Right
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#include "Variables.h"

#include "OTA.h"  //Need some work to add timeout
#include "Battery.h"
#include "Buttons.h"
#include "ComputeRCData.h"
#include "EEPROM.h"
//#include "FrSkyX.h" //Need some work to add bind and rx num functionnality
#include "Mavlink2.h" //Need some work to add rc link & telemetry
#include "Navigation.h" //Need some work to add highlights
#include "SBus.h"
#include "SH1106.h"
#include "ShowSketchName.h"
#include "SoftPower.h"  //Need some work 
#include "XBMP.h"

//===============================================================================================================================================================================================================
//----------------------------------------------------------------------------------------------------SETUP------------------------------------------------------------------------------------------------------
//===============================================================================================================================================================================================================
void setup(void){
  
  Wire.begin();   // Initialise I2C communication as MASTER
  Serial.begin(115200);  //Initialise Serial 1
  EEPROM.begin(12); //Initialise EEPROM and Ask for 12 addresses
  u8g2.begin(); //Initialise SH1106
  mlx.begin();  //Initialise MLX

  FirstBoot();
  ShowSketchName(); 
  OTASetup();
  PinModeDef();
  ReadEEPROM();  
  SBusInit();

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
  Screen();
  ReadVoltage();
  Serial.println(Pwr.State);

}
