#include "Wire.h"
#include "display.h"

#define I2C_SDA 23
#define I2C_SCL 18
#define BL 22

display display(BL);

void setup()
{
  Serial.begin(115200);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BL, 0);
  ledcWrite(0, 255);

  Wire.begin(I2C_SDA, I2C_SCL);
  delay(1000);

  display.begin();

  delay(6);

}

void loop()
{
  display.set_left_graph(3, 0);
  display.set_right_graph(2, 0);
  display.set_rc_rssi(0, 1);
  display.set_named_rssi(0, 1);
  display.set_tx_battery_bar(3, 1);
  display.set_tx_battery_percentage(100, 1);
  display.set_drone_battery_bar(3, 1);
  display.set_drone_battery_percentage(100, 1);
  display.set_rpm(0, 1);
  display.set_speed(0, 1);
  display.set_distance(0, 1);
  display.set_altitude(4807, 1);
  display.set_clearance(0, 1);
  display.set_ev(0, 0);
  display.set_sd(0);
  display.set_sport(0);
  display.set_vision(0);
  display.set_rec(0);
  display.set_name(" gps", 750);
  display.set_text("  puma tx", 750);
  
  display.update_display();
  
  delay(100);

}
