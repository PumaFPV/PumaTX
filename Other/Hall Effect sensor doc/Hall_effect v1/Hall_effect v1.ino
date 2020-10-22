
//include
#include "Arduino.h"
#include <Wire.h>
#include "MLX90393Right.h"
#include "MLX90393Left.h"

MLX90393Right rmlx;
MLX90393Left lmlx;
MLX90393Right::txyz datar;
MLX90393Left::iabc datal;

//setup
void setup(){
  Serial.begin(9600);
  Wire.begin();
  rmlx.begin();
  lmlx.begin();
 
}
//Main
void loop(){
  
}

