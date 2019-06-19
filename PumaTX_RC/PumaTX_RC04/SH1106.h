#ifndef SH1106_h
#define SH1106_h

#include "XBMP.h"




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
  //u8g2.print(Arm.Output);
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
if (page == 5){ //--------------------------------------------------Page-5--------------------------------------------------
  u8g2.clear();
  esp_deep_sleep_start();
  }
}

void Screen(){
  
    u8g2.firstPage();  
    do {
      DrawScreen();
    } while ( u8g2.nextPage() );
    
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

#endif
