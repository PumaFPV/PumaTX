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
#include "crsf.h"

#include "variables.h"

#include "buttons.h"
#include "rc.h"
#include "menu.h"
#include "haptic.h"


uint8_t crsfPacket[CRSF_PACKET_SIZE];
uint8_t crsfCmdPacket[CRSF_CMD_PACKET_SIZE];
int16_t rcChannels[CRSF_MAX_CHANNEL];
uint32_t crsfTime = 0;

void setup() 
{

  debug.begin(115200); //Start Serial connection
  crsf.begin(400000, SERIAL_8N1, CRSF_UART, CRSF_UART, false, 500);
  mlxI2C.begin(PUMATX_SDA, PUMATX_SCL, 1000000L); //Start I2C connection
  displayI2C.begin(DISPLAY_SDA, DISPLAY_SCL, 1000000L);

  //-----MLX
  mlx.begin();

  //-----Button
  pinModeDef(); //Defines every buttons


  //-----RC


  //-----CRSF
  crsfClass.begin();

  //-----Display / Menu
  display.begin();
  //display.displayDefault();
  display.off();
  display.setText("  pumatx");
  display.update();

  //-----Haptic
  hapticSetup();
  delay(500);

}




void loop()
{

  if(mlxTask.counter == 0)
  {
    mlxTask.startCounterTime = micros();
  }
  if(micros() - mlxTask.startCounterTime > MIRCOS2SECONDS)
  {
    mlxTask.frequency = mlxTask.counter;
    //debug.println(mlxTask.counter);
    mlxTask.counter = 0;
  }   

  if(buttonTask.counter == 0)
  {
    buttonTask.startCounterTime = micros();
  }
  if(micros() - buttonTask.startCounterTime > MIRCOS2SECONDS)
  {
    buttonTask.frequency = buttonTask.counter;
    //debug.println(buttonTask.counter);
    buttonTask.counter = 0;
  }

  if(rcTask.counter == 0)
  {
    rcTask.startCounterTime = micros();
  }
  if(micros() - rcTask.startCounterTime > MIRCOS2SECONDS)
  {
    rcTask.frequency = rcTask.counter;
    //debug.println(rcTask.counter);
    rcTask.counter = 0;
  }

  if(crsfTask.counter == 0)
  {
    crsfTask.startCounterTime = micros();
  }
  if(micros() - crsfTask.startCounterTime > MIRCOS2SECONDS)
  {
    crsfTask.frequency = crsfTask.counter;
    //debug.println(crsfTask.counter);
    crsfTask.counter = 0;
  }

  if(menuTask.counter == 0)
  {
    menuTask.startCounterTime = micros();
  }
  if(micros() - menuTask.startCounterTime > MIRCOS2SECONDS)
  {
    menuTask.frequency = menuTask.counter;
    //debug.println(menuTask.counter);
    menuTask.counter = 0;
  }

  if(hapticTask.counter == 0)
  {
    hapticTask.startCounterTime = micros();
  }
  if(micros() - hapticTask.startCounterTime > MIRCOS2SECONDS)
  {
    hapticTask.frequency = hapticTask.counter;
    //debug.println(hapticTask.counter);
    hapticTask.counter = 0;
  }

  if(hapticTask.counter == 0)
  {
    hapticTask.startCounterTime = micros();
  }
  if(micros() - hapticTask.startCounterTime > 1000000)
  {
    hapticTask.frequency = hapticTask.counter;
    //debug.println(hapticTask.counter);
    hapticTask.counter = 0;
  }


  //-----Button
  if(micros() - buttonTask.beginTime >= buttonTask.interval)
  {
    buttonTask.beginTime = micros();
    buttonTask.inBetweenTime = buttonTask.beginTime - buttonTask.endTime;

    processRcButtons(); //reads 16 inputs

    buttonTask.endTime = micros();
    buttonTask.counter++;
    buttonTask.duration = buttonTask.endTime - buttonTask.beginTime;
    //debug.print("button time: ");
    //debug.println(buttonTask.duration); //355us with 16 readings, 230us with rc necessary
  }


  //-----MLX
  if(micros() - mlxTask.beginTime >= mlxTask.interval)
  {
    mlxTask.beginTime = micros();
    mlxTask.inBetweenTime = mlxTask.beginTime - mlxTask.endTime;

    mlx.process();
    getMlxData();

    mlxTask.endTime = micros();
    mlxTask.counter++;
    mlxTask.duration = mlxTask.endTime - mlxTask.beginTime;
    //debug.print("mlx time: ");
    //debug.println(mlxTask.duration); //940us
  }


  //-----RC
  if(micros() - rcTask.beginTime >= rcTask.interval) //40ms->25Hz
  {
    rcTask.beginTime = micros();
    rcTask.inBetweenTime = rcTask.beginTime - rcTask.endTime;

    computeRc();
    rcData(); 

    rcTask.endTime = micros();
    rcTask.counter++;
    rcTask.duration = rcTask.endTime - rcTask.beginTime;
    //debug.print("rc time: ");
    //debug.println(rcTask.duration); //17us

    /*
    for(uint8_t i = 0; i<15; ++i)
    {
      debug.print(channels[i]);
      debug.print(" ");
    }
    debug.println();
    */

  } 



  //-----CRFS
  if(micros() - crsfTask.beginTime >= crsfTask.interval)
  {
    crsfTask.beginTime = micros();
    crsfTask.inBetweenTime = crsfTask.beginTime - crsfTask.endTime;

    crsfClass.crsfPrepareDataPacket(crsfPacket, channels);
    crsfClass.CrsfWritePacket(crsfPacket, CRSF_PACKET_SIZE);

    crsfTask.endTime = micros();
    crsfTask.counter++;
    crsfTask.duration = crsfTask.endTime - crsfTask.beginTime;

  }


  //-----Display / Menu
  if(micros() - menuTask.beginTime >= menuTask.interval)  //20Hz
  {
    menuTask.beginTime = micros();
    menuTask.inBetweenTime = menuTask.beginTime - menuTask.endTime;

    processNavButtons();
    navigation();
    computeBattery();
    computeThrottle();
    computePitch();
    if(page != lastPage || line != lastLine || update != lastUpdate)
    {
      lastPage = page;
      lastLine = line;
      lastUpdate = update;
      displayTxBattery();
      menuHandler();
      display.update();
    }

    menuTask.endTime = micros();
    menuTask.counter++;
    menuTask.duration = menuTask.endTime - menuTask.beginTime;
    //debug.print("display duration time: ");
    //debug.println(menuLoop.duration); //3200us@240kHz without button reading / 1300us@1MHz with button reading / 1400 when updating, 118@CPU240MHz when not 214 @CPU80MHz
  }  


  if(micros() - hapticTask.beginTime >= hapticTask.interval)
  {
    hapticTask.beginTime = micros();
    hapticTask.inBetweenTime = hapticTask.beginTime - hapticTask.endTime;

    hapticHandler(hapticDuty);

    hapticTask.endTime = micros();
    hapticTask.counter++;
    hapticTask.duration = hapticTask.endTime - hapticTask.beginTime;

  }

}


/* Typical task outline

  //Task
  if(micros() - Task.beginTime >= Task.interval)
  {
    Task.beginTime = micros();
    Task.inBetweenTime = Task.beginTime - Task.endTime;

    **functions

    Task.endTime = micros();
    Task.counter++;
    Task.duration = Task.endTime - Task.beginTime;

  }

  //Task frequency counter
  if(Task.counter == 0)
  {
    Task.startCounterTime = micros();
  }
  if(micros() - Task.startCounterTime > 1000000)
  {
    Task.frequency = Task.counter;
    debug.println(Task.counter);
    Task.counter = 0;
  }

*/
