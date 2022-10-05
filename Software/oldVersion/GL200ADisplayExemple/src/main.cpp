#include <Arduino.h>
#include <Wire.h>

#include "GL200ADisplay.h"

#define SDA 23
#define SCL 18
#define I2CFrequency 400000UL

GL200ADisplay display(/*displayI2C,*/ 22);

void setup() 
{
  Serial.begin(115200);
  Wire.begin(SDA, SCL, I2CFrequency); //Starts I2C connection
  display.begin();
  delay(1000);
  display.displayDefault();
  display.update();
  delay(1000);

}

void loop() 
{
  display.begin();
  delay(100);
/*   int i = 1;
  display.setLeftGraph(6-i%6, 0);
  display.setRightGraph(i%6, 0);
  display.setRcRssi(6-i%6, 1);
  display.setNamedRssi(i%6, 1);
  display.setTxBatteryBar(i%4, 1);
  display.setTxBatteryPercentage(i, 1);
  display.setDroneBatteryBar(i%6, 1);
  display.setDroneBatteryPercentage(i, 1);
  display.setRpm(i, 1);
  display.setSpeed(i, 1);
  display.setDistance(i, 1);
  display.setAltitude(i, 1);
  display.setClearance(i, 1);
  display.setEv(0, 0);
  display.setSd(0);
  display.setSport(0);
  display.setVision(0);
  display.setRec(0);
  display.setName(" gps", 750);
  display.setText("  puma tx", 750);
 */
  display.displayDefault();

  display.update();
  Serial.println("loop");
  //i++;
  
  delay(250);
}