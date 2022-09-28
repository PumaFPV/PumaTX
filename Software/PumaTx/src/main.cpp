#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "rc.h"
#include "menu.h"
//#include "dualcore.h"



//-----MLX
unsigned long previousMlxMillis = 0;
const long mlxInterval = 10;  

//-----Button
unsigned long previousButtonMillis = 0;
const long buttonInterval = 10;

//-----Display / Menu
unsigned long previousMenuMillis = 0;
const long menuInterval = 50;

//-----RC
unsigned long previousRcMillis = 0;
const long rcInterval = 40;




void setup() 
{
  Serial.begin(115200); //Starts Serial connection
  crsf.begin(400000, SERIAL_8N1, CRSF, CRSF, false, 500);
  Wire.begin(PUMATX_SDA, PUMATX_SCL); //Starts I2C connection
  
  //-----MLX
  mlx.begin();

  //-----Button
  pinModeDef(); //Defines every buttons

  //-----RC


  //-----Display / Menu
  display.begin();
  display.displayDefault();

}


void loop()
{
  unsigned long currentMlxMillis = millis();
  unsigned long currentButtonMillis = millis();
  unsigned long currentMenuMillis = millis();
  unsigned long currentRcMillis = millis();

  //-----MLX
  if(currentMlxMillis - previousMlxMillis >= mlxInterval)
  {
    previousMlxMillis = currentMlxMillis;
    
    mlx.process();
    getMlxData();  
  }

  //-----Button
  if(currentButtonMillis - previousButtonMillis >= buttonInterval)
  {
    previousButtonMillis = currentButtonMillis;

    processButtons();
  }

  //-----RC
  if(currentRcMillis - previousRcMillis >= rcInterval)
  {
    previousRcMillis = currentRcMillis;

    computeRc();
    rcData(); 
  } 

  //-----Display / Menu
  if(currentMenuMillis - previousMenuMillis >= menuInterval)
  {
    previousMenuMillis = currentMenuMillis;

    navigation();
    menuLoop();
  }  

}
