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
#include "haptic.h"

void setup() 
{

  debug.begin(115200); //Start Serial connection
  crsf.begin(400000, SERIAL_8N1, CRSF, CRSF, false, 500);
  mlxI2C.begin(PUMATX_SDA, PUMATX_SCL, 1000000L); //Start I2C connection
  displayI2C.begin(DISPLAY_SDA, DISPLAY_SCL, 1000000L/*240000L*/);

  //-----MLX
  mlx.begin();

  //-----Button
  pinModeDef(); //Defines every buttons


  //-----RC


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
    mlxTask.startTime = micros();
  }
  if(micros() - mlxTask.startTime > 1000000)
  {
    mlxTask.frequency = mlxTask.counter;
    //debug.println(mlxTask.counter);
    mlxTask.counter = 0;
    mlxTask.startTime = micros();
  }

  if(buttonTask.counter == 0)
  {
    buttonTask.startTime = micros();
  }
  if(micros() - buttonTask.startTime > 1000000)
  {
    buttonTask.frequency = buttonTask.counter;
    //debug.println(buttonTask.counter);
    buttonTask.counter = 0;
    buttonTask.startTime = micros();
  }

  if(rcTask.counter == 0)
  {
    rcTask.startTime = micros();
  }
  if(micros() - rcTask.startTime > 1000000)
  {
    rcTask.frequency = rcTask.counter;
    //debug.println(rcTask.counter);
    rcTask.counter = 0;
    rcTask.startTime = micros();
  }

  if(crsfTask.counter == 0)
  {
    crsfTask.startTime = micros();
  }
  if(micros() - crsfTask.startTime > 1000000)
  {
    crsfTask.frequency = crsfTask.counter;
    //debug.println(crsfTask.counter);
    crsfTask.counter = 0;
    crsfTask.startTime = micros();
  }

  if(menuTask.counter == 0)
  {
    menuTask.startTime = micros();
  }
  if(micros() - menuTask.startTime > 1000000)
  {
    menuTask.frequency = menuTask.counter;
    //debug.println(menuTask.counter);
    menuTask.counter = 0;
    menuTask.startTime = micros();
  }

  if(hapticTask.counter == 0)
  {
    hapticTask.startTime = micros();
  }
  if(micros() - hapticTask.startTime > 1000000)
  {
    hapticTask.frequency = hapticTask.counter;
    //debug.println(hapticTask.counter);
    hapticTask.counter = 0;
    hapticTask.startTime = micros();
  }



  currentTime = micros();


  //-----Button
  if(currentTime - buttonTask.previousTime >= buttonTask.interval)
  {
    buttonTask.previousTime = micros();
    buttonTask.inBetweenTime = buttonTask.previousTime - buttonTask.endTime;
    buttonTask.beginTime = micros();

    processRcButtons(); //reads 16 inputs

    buttonTask.endTime = micros();
    buttonTask.counter++;
    buttonTask.duration = buttonTask.endTime - buttonTask.beginTime;
    //debug.print("button time: ");
    //debug.println(buttonTask.duration); //355us with 16 readings, 230us with rc necessary
  }



  //-----MLX
  if(currentTime - mlxTask.previousTime >= mlxTask.interval)
  {
    mlxTask.previousTime = micros();
    mlxTask.inBetweenTime = mlxTask.previousTime - mlxTask.endTime;
    mlxTask.beginTime = micros();

    mlx.process();
    getMlxData();

    mlxTask.endTime = micros();
    mlxTask.counter++;
    mlxTask.duration = mlxTask.endTime - mlxTask.beginTime;
    //debug.print("mlx time: ");
    //debug.println(mlxTask.duration); //730us
  }



  //-----RC
  if(currentTime - rcTask.previousTime >= rcTask.interval) //40ms->25Hz
  {
    rcTask.previousTime = micros();
    rcTask.inBetweenTime = rcTask.previousTime - rcTask.endTime;
    rcTask.beginTime = micros();

    computeRc();
    rcData(); 

    rcTask.endTime = micros();
    rcTask.counter++;
    rcTask.duration = rcTask.endTime - rcTask.beginTime;
    //debug.print("rc time: ");
    //debug.println(rcTask.duration); //17us

  } 
  /*
  for(uint8_t i = 0; i<15; ++i)
  {
    debug.print(channels[i]);
    debug.print(" ");
  }
  debug.println();
*/

  //-----Display / Menu
  if(currentTime - menuTask.previousTime >= menuTask.interval)  //20Hz
  {
    menuTask.previousTime = micros();
    menuTask.inBetweenTime = menuTask.previousTime - menuTask.endTime;
    menuTask.beginTime = micros();

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
      display.off();
      displayTxBattery();
      display.setRpm(mlxTask.duration, 0);
      menuHandler();
      display.update();
    }

    menuTask.endTime = micros();
    menuTask.counter++;
    menuTask.duration = menuTask.endTime - menuTask.beginTime;
    //debug.print("display duration time: ");
    //debug.println(menuLoop.duration); //3200us@240kHz without button reading / 1300us@1MHz with button reading / 1400 when updating, 118@CPU240MHz when not 214 @CPU80MHz
  }  


  hapticHandler();

}


/* Typical task outline

  //Task
  if(currentTime - Task.previousTime >= Task.interval)
  {
    Task.inBetweenTime = Task.previousTime - Task.endTime;
    Task.previousTime = currentTime;
    Task.beginTime = micros();

    **functions

    Task.endTime = micros();
    Task.counter++;
    Task.duration = Task.endTime - Task.beginTime;

  }

  //Task frequency counter
  if(Task.counter == 0)
  {
    Task.startTime = micros();
  }
  if(micros() - Task.startTime > 1000000)
  {
    Task.frequency = Task.counter;
    debug.println(Task.counter);
    Task.counter = 0;
    Task.startTime = micros();
  }

*/
