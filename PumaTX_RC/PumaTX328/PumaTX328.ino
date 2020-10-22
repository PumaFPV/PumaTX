#include <Arduino.h>
#include <SoftwareSerial.h>

#include "PXX.h"

SoftwareSerial esp32(2,3); //Rx, Tx

int16_t channels[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int rx_number = 18;
bool prepare = true;

void setup(){
    esp32.begin(115200);
    //Serial.begin(115200);
    //Serial.begin(9600);
    PXX.begin();
}

void loop() {
  receiveData();
  //Serial.println(esp32.read());
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

void receiveData(){
  if(esp32.available() > 0){
    for(int i = 0; i < 10; i++){
      channels[i] = esp32.read();
    }
  }
}
