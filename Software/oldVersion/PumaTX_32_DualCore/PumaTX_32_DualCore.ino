#include <Arduino.h>

#include "variables.h"

#include "buttons.h"
#include "computeRCdata.h"
#include "R9M.h"
#include "tasks.h"



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
  setup_task();
  
  Serial.begin(115200); //Starts Serial connection

}



void loop() {

}
