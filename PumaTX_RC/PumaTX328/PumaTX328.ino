#include <Arduino.h>

#include "PXX.h"


int16_t channels[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int rx_number = 18;
bool prepare = true;

void setup(){
    //Serial.begin(115200);
    //Serial.begin(9600);
    PXX.begin();
}

void loop() {
  test_data();
  loop_pxx();

}

void loop_pxx(){
  
    if(prepare){
            PXX.prepare(channels, 0x12, 0x00);    
    }
    else
    {
        PXX.send();
    }

    prepare = !prepare;

    delay(2);
    
}

void test_data(){
  for(int i = 0; i < 100; i++){
    channels[0] = i;
  }
  //channels[0] = 10;
  channels[1] = 0;
  channels[2] = 0;
  channels[3] = 0;
  channels[4] = 0;
  channels[5] = 0;
  channels[6] = 0;
  channels[7] = 0;

}
