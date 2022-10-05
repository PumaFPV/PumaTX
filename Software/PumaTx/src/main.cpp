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
  Serial.println("setup");
  crsf.begin(400000, SERIAL_8N1, CRSF, CRSF, false, 500);
  mlxI2C.begin(PUMATX_SDA, PUMATX_SCL); //Starts I2C connection
  displayI2C.begin(DISPLAY_SDA, DISPLAY_SCL);

  //-----MLX
  Serial.println("mlx setup");
  mlx.begin();

  //-----Button
  Serial.println("pinMode setup");
  pinModeDef(); //Defines every buttons

  //-----RC


  //-----Display / Menu
  Serial.println("display setup");
  display.begin();
  Serial.println("display begin done");
  display.displayDefault();


}


void loop()
{

  currentTime = millis();
  

  Serial.println("loop");
 
  //-----MLX
  if(currentTime - previousMlxMillis >= mlxInterval)
  {
    previousMlxMillis = currentMlxMillis;
    
    mlx.process();
    getMlxData();  
  }

  //-----Button
  if(currentTime - previousButtonMillis >= buttonInterval)
  {
    previousButtonMillis = currentButtonMillis;

    processButtons();
  }

  //-----RC
  if(currentTime - previousRcMillis >= rcInterval)
  {
    previousRcMillis = currentRcMillis;

    computeRc();
    rcData(); 
  } 

  //-----Display / Menu
  if(currentTime - previousMenuMillis >= menuInterval)
  {
    previousMenuMillis = currentMenuMillis;

    navigation();
    menuLoop();
  }  

}
