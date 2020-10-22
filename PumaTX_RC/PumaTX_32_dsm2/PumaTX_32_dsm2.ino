#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "computeRCdata.h"

#define BIND_SWITCH  Pwr.Pin
#define BIND_LED     LED.Pin

DSM2_tx tx(6);

void bind_cb( int state, byte model_id ) {
  if( state == 1 ) {
    digitalWrite(BIND_LED, LOW);
  }
  else {
    digitalWrite(BIND_LED, HIGH);
  }
  if( state == 2 ) {
    //digitalWrite(ERROR_LED, HIGH);
  }
}

void GetSketchName(){
  
  String path = __FILE__;
  int slash = path.lastIndexOf('\x5C');
  String the_cpp_name = path.substring(slash+1);
  int dot_loc = the_cpp_name.lastIndexOf('.');
  Firmware = the_cpp_name.substring(0, dot_loc);
  //Serial.println(Firmware);
    
}

void setup() {

  GetSketchName();  //Outputs the firmware name used
  //Serial.begin(115200); //Starts Serial connection
  
  Wire.begin(I2C_SDA, I2C_SCL); //Starts I2C connection
  //mlx.begin();  //Starts mlx communication 
  SetupLeftMLX();
  SetupRightMLX();

  PinModeDef(); //Defines every buttons
  //delay(5000);
    tx.begin();
  //Serial.println(digitalRead(BIND_SWITCH));
  if( digitalRead(BIND_SWITCH) == LOW ) {
    //Serial.println("bind begin");
    tx.bind(bind_cb, 0);
    //Serial.println("bind end");
  }
}

void loop() {

  LoopLeftMLX();
  delay(5);
  LoopRightMLX();
  ProcessButtons();
  ComputeRC4();
  RCdata();
  
  for( byte i = 0; i < tx.channel_count; i++ ){
    tx.set_channel(i, channels[i], 100, -100);
  }
  tx.send_frame();
  
/*
  for(int i = 0; i < 10; i++){
    Serial.print(channels[i]);
    Serial.print("  ");
  }
  Serial.println();
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
