#include <TimerOne.h>
#include "SBUS.h"
SBUS x8r(Serial);  // a SBUS object, which is on hardware serial port 1
uint16_t ain[10]; // analog read values, 16 bit counts
void setup() {
Serial.begin(115200); // serial to display the channel commands for debugging
x8r.begin(); // begin the SBUS communication
//analogReadResolution(16);  // setup the analog read resolution to 16 bits
Timer1.initialize(9000);  // setup an interrupt to send packets every 9 ms
Timer1.attachInterrupt(sendSBUS);
}
void loop() {
}
void sendSBUS() {  /* reads analog inputs and sends an SBUS packet */
  float scaleFactor = 1639.0f / 65535.0f;
  float bias = 172.0f;
  uint16_t channels[16];

  // read the analog inputs
  for(uint8_t i = 14; i < 24; i++) {
    ain[i-14] = analogRead(i);
  }
  // linearly map the analog measurements (0-65535) to the SBUS commands (172-1811)
  for(uint8_t i = 0; i < 10; i++) {
    channels[i] = (uint16_t)(((float)ain[i]) * scaleFactor + bias);
    Serial.print(channels[i]); // print the channel command (172-1811)
    Serial.print("\t");
  }
  Serial.println();
  x8r.write(&channels[0]);   // write the SBUS packet to an SBUS compatible servo
}
