#include <Arduino.h>

#include "PXX.h"


int16_t channels[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int rx_number = 18;
bool prepare = true;

void setup(){
    PXX.begin();
    test_data();
}

void loop() {
  loop_pxx();

}

void loop_pxx(){
  
    if(prepare){
            PXX.prepare(channels, rx_number, 0x00);    
    }
    else
    {
        PXX.send();
    }

    prepare = !prepare;

    delay(2);
    
}

void test_data(){
  channels[0] = -100;
  channels[1] = 0;
  channels[2] = 0;
  channels[3] = 0;
  channels[4] = 0;
  channels[5] = 10;
  channels[6] = 0;
  channels[7] = 100;

}
