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


void scannerMlx();
void scannerDisplay();

void setup() 
{
  //ledcSetup(1, 40000, 8);
  //ledcAttachPin(HAPTIC_PWM, 1);
  //ledcWrite(1, 40);
  Serial.begin(115200); //Start Serial connection
  Serial.println("setup");
  //crsf.begin(400000, SERIAL_8N1, CRSF, CRSF, false, 500);
  mlxI2C.begin(PUMATX_SDA, PUMATX_SCL); //Start I2C connection
  displayI2C.begin(DISPLAY_SDA, DISPLAY_SCL, 240000L);

  //-----MLX
  Serial.println("mlx setup");
 // mlx.begin();

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
  
  //-----MLX
  if(currentTime - previousMlxMillis >= mlxInterval)
  {
    previousMlxMillis = currentTime;
    unsigned long mlxBeginTime = micros();

    mlx.process();
    getMlxData(); 

    unsigned long mlxEndTime = micros();
    //Serial.println("mlx time: ");
    //Serial.println(mlxEndTime - mlxBeginTime); 
  }

  //-----Button
  if(currentTime - previousButtonMillis >= buttonInterval)
  {
    previousButtonMillis = currentTime;
    unsigned long buttonBeginTime = micros();

    processButtons();

    unsigned long buttonEndTime = micros();
    //Serial.println("button time: ");
    //Serial.println(buttonEndTime - buttonBeginTime); 
  }

  //-----RC
  if(currentTime - previousRcMillis >= rcInterval)
  {
    previousRcMillis = currentTime;
    unsigned long rcBeginTime = micros();

    computeRc();
    rcData(); 

    unsigned long rcEndTime = micros();
    //Serial.println("rc time: ");
    //Serial.println(rcEndTime - rcBeginTime); 

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
  if(currentTime - previousMenuMillis >= menuInterval)
  {
    previousMenuMillis = currentTime;
    unsigned long displayBeginTime = micros();

    navigation();
    menuLoop();

    unsigned long displayEndTime = micros();
    //Serial.println("display time: ");
    //Serial.println(displayEndTime - displayBeginTime); 
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