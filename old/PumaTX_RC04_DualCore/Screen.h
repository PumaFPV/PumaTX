#ifndef Screen_h
#define Screen_h

#include "XBMP.h"

#include "Pages\Page0.h"
#include "Pages\Page1.h"
#include "Pages\Page2.h"
#include "Pages\Page3.h"
#include "Pages\Page4.h"
#include "Pages\Page5.h"




void DrawScreen(void) {
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.setCursor(123, 57);
  u8g2.print(page);


  if (page == 0){ //--------------------------------------------------Page-0--------------------------------------------------
    Page0();
  }

  if (page == 1){ //--------------------------------------------------Page-1--------------------------------------------------
    Page1();
  }

  if (page == 2){ //--------------------------------------------------Page-2--------------------------------------------------
    Page2();
  }


  if (page == 3){ //--------------------------------------------------Page-3--------------------------------------------------
    Page3();
  }


if (page == 4){ //--------------------------------------------------Page-4-------------------------------------------------- 
  Page4();
  }
if (page == 5){ //--------------------------------------------------Page-5--------------------------------------------------
    Page5();
  }
}

void ScreenLoop(){
  
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
    ScreenLoop();
  }
  
}

#endif
