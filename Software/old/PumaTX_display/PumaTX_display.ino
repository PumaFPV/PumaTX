#include "Wire.h"
#include "display.h"

#define BL 22

display display(BL);

int i;

void setup()
{

  Serial.begin(115200);

  //delay(1000);
  Wire.begin(17,13,400000);
  //delay(1000);
  display.begin();  
  display.defaults();
  display.update();


  delay(6);

}

void loop()
{
  /*
  //display.begin();
  
  display.set_left_graph(6-i%6, 0);
  display.set_right_graph(i%6, 0);
  display.set_rc_rssi(6-i%6, 1);
  display.set_named_rssi(i%6, 1);
  display.set_tx_battery_bar(i%4, 1);
  display.set_tx_battery_percentage(i, 1);
  display.set_drone_battery_bar(i%6, 1);
  display.set_drone_battery_percentage(i, 1);
  display.set_rpm(i, 1);
  display.set_speed(i, 1);
  display.set_distance(i, 1);
  display.set_altitude(i, 1);
  display.set_clearance(i, 1);
  display.set_ev(0, 0);
  display.set_sd(0);
  display.set_sport(0);
  display.set_vision(0);
  display.set_rec(0);
  display.set_name(" gps", 750);
  display.set_text("  puma tx", 750);

  display.update();
  Serial.println("loop");
  i++;
  
  delay(100);
*/
}
