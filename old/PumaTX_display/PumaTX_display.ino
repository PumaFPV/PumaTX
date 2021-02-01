#include "Wire.h"
#include "display.h"

#define BL PB5

display display(BL);

int i;

void setup()
{

  Serial.begin(115200);

  //delay(1000);
  Wire.begin();
  //delay(1000);
  display.begin();  

  delay(6);

}

void loop()
{
  
  display.begin();
  
  display.set_left_graph(3, 0);
  display.set_right_graph(2, 0);
  display.set_rc_rssi(i, 1);
  display.set_named_rssi(0, 1);
  display.set_tx_battery_bar(3, 1);
  display.set_tx_battery_percentage(i, 1);
  display.set_drone_battery_bar(3, 1);
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
  
  display.update_display();
  Serial.println("loop");
  i++;
  
  delay(100);

}
