/*

 /\\\\\\\\\\\\\                                                     /\\\\\\\\\\\\\\\  /\\\       /\\\         
 \/\\\/////////\\\                                                  \///////\\\/////  \///\\\   /\\\/         
  \/\\\       \/\\\                                                        \/\\\         \///\\\\\\/          
   \/\\\\\\\\\\\\\/   /\\\    /\\\    /\\\\\  /\\\\\    /\\\\\\\\\          \/\\\           \//\\\\         
    \/\\\/////////    \/\\\   \/\\\  /\\\///\\\\\///\\\ \////////\\\         \/\\\            \/\\\\    
     \/\\\             \/\\\   \/\\\ \/\\\ \//\\\  \/\\\   /\\\\\\\\\\        \/\\\            /\\\\\\   
      \/\\\             \/\\\   \/\\\ \/\\\  \/\\\  \/\\\  /\\\/////\\\        \/\\\          /\\\////\\\  
       \/\\\             \//\\\\\\\\\  \/\\\  \/\\\  \/\\\ \//\\\\\\\\/\\       \/\\\        /\\\/   \///\\\ 
        \///               \/////////   \///   \///   \///   \////////\//        \///        \///       \///
   
*/                             

//--------------------------------------------------Include libraries--------------------------------------------------
#include <Arduino.h>
#include <Wire.h>
#include "mlx.h"
#include "GL200ADisplay.h"

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


void scannerMlx();
void scannerDisplay();

void setup() 
{
  //ledcSetup(1, 40000, 8);
  //ledcAttachPin(HAPTIC_PWM, 1);
  //ledcWrite(1, 40);
  debug.begin(115200); //Start Serial connection
  debug.println("setup");
  //crsf.begin(400000, SERIAL_8N1, CRSF, CRSF, false, 500);
  mlxI2C.begin(PUMATX_SDA, PUMATX_SCL, 1000000L); //Start I2C connection
  displayI2C.begin(DISPLAY_SDA, DISPLAY_SCL, 1000000L/*240000L*/);

  //-----MLX
  debug.println("mlx setup");
  mlx.begin();

  //-----Button
  debug.println("pinMode setup");
  pinModeDef(); //Defines every buttons

  //-----RC


  //-----Display / Menu
  debug.println("display setup");
  display.begin();
  debug.println("display begin done");
  //display.displayDefault();
  display.off();
  delay(1000);
  display.setText("  pumatx");
  display.update();
  delay(2000);
}


void loop()
{

  currentTime = millis();
  //-----MLX
  if(currentTime - previousMlxMillis >= mlxInterval)
  {
    previousMlxMillis = currentTime;
    unsigned long mlxBeginTime = micros();

    mlx.process();
    getMlxData(); 

    unsigned long mlxEndTime = micros();
    //Serial.print("mlx time: ");
    //Serial.println(mlxEndTime - mlxBeginTime); //740
  }

  //-----Button
  if(currentTime - previousButtonMillis >= buttonInterval)
  {
    previousButtonMillis = currentTime;
    unsigned long buttonBeginTime = micros();

    processRcButtons(); //reads 16 inputs

    unsigned long buttonEndTime = micros();
    //Serial.print("button time: ");
    //Serial.println(buttonEndTime - buttonBeginTime); //355us with 16 readings, 230us with rc necessary
  }

  //-----RC
  if(currentTime - previousRcMillis >= rcInterval)
  {
    previousRcMillis = currentTime;
    unsigned long rcBeginTime = micros();

    computeRc();
    rcData(); 

    unsigned long rcEndTime = micros();
    //Serial.print("rc time: ");
    //Serial.println(rcEndTime - rcBeginTime); //17us

  } 
  /*
  for(uint8_t i = 0; i<15; ++i)
  {
    Serial.print(channels[i]);
    Serial.print(" ");
  }
  Serial.println();
*/


  //-----Display / Menu
  if(currentTime - previousMenuMillis >= menuInterval)  //20Hz
  {
    previousMenuMillis = currentTime;
    unsigned long displayBeginTime = micros();


    processNavButtons();
    navigation();
    computeBattery();
    computeThrottle();

    if(page != lastPage || line != lastLine || update != lastUpdate)
    {
      lastPage = page;
      lastLine = line;
      lastUpdate = update;
      display.off();
      displayTxBattery();
      menuLoop();
      display.update();

    }



    unsigned long displayEndTime = micros();

    debug.print("display time: ");
    debug.println(displayEndTime - displayBeginTime); //3200us@240kHz without button reading / 1300us@1MHz with button reading / 1400 when updating, 118@CPU240MHz when not 214 @CPU80MHz
  }  

  //scannerMlx();
  //scannerDisplay();
  //delay(1000);
}

void scannerMlx()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  mlxI2C.begin();
  for (byte i = 8; i < 120; i++)
  {
    mlxI2C.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (mlxI2C.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s) on mlxI2C");
}

void scannerDisplay()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  displayI2C.begin();
  for (byte i = 8; i < 120; i++)
  {
    displayI2C.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (displayI2C.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s) on displayI2C");
}