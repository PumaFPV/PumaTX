#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "computeRCdata.h"
#include "R9M.h"

void GetSketchName(){
  
  String path = __FILE__;
  int slash = path.lastIndexOf('\x5C');
  String the_cpp_name = path.substring(slash+1);
  int dot_loc = the_cpp_name.lastIndexOf('.');
  Firmware = the_cpp_name.substring(0, dot_loc);
  Serial.println(Firmware);
    
}

void setup() {
  GetSketchName();  //Outputs the firmware name used
  Serial.begin(115200); //Starts Serial connection
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection
  //mlx.begin();  //Starts mlx communication 
  SetupLeftMLX();
  SetupRightMLX();

  PinModeDef(); //Defines every buttons

  setup_PXX(0x12, EU_10_mw); //Rx number, power and zone
}

void loop() {

  LoopLeftMLX();
  
  //mlx.process();
  ProcessButtons();
  ComputeRC4();
  RCdata();
  loop_PXX();
  LoopRightMLX();


  
/*
  for(int i = 0; i < 10; i++){
    Serial2.write(channels[i]);
  }
  //Serial2.println();
*/

/*
Serial.print(Throttle.Output);
Serial.print("  ");
Serial.print(Pitch.Output);
Serial.print("  ");
Serial.print(Roll.Output);
Serial.print("  ");
Serial.print(Yaw.Output);
Serial.print("  ");
Serial.print(RightPot.Output);
Serial.print("  ");
Serial.print(LeftPot.Output);
Serial.print("  ");
Serial.print(Arm.Output);
Serial.print("  ");
Serial.print(Pre.Output);
Serial.print("  ");
Serial.print(RTH.Output);
Serial.print("  ");
Serial.print(Pause.Output);
Serial.print("  ");
Serial.print(Pwr.State);
Serial.println("  ");
*/
/*
Serial.print(Ok.State);
Serial.print("  ");
Serial.print(Up.State);
Serial.print("  ");
Serial.print(Down.State);
Serial.print("  ");
Serial.print(Right.State);
Serial.print("  ");
Serial.print(Left.State);
Serial.print("  ");

Serial.println();
*/
}
