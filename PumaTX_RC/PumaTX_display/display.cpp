/*
	display.cpp - Library made to write to a DJI Mavic Pro controller (GL200A) display using I2C
	dont forget to initialise I2C with this command:   Wire.begin(I2C_SDA, I2C_SCL);
	
	Created by Nicolas de Grave, November 23rd, 2020
*/

#include "Arduino.h"
#include "display.h"
#include "Wire.h"

display::display(uint8_t BL_pin)
{
	_BL = BL_pin;
}


void display::begin()  //setup function. has to be called to init program
{
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x12);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0xFE);
  Wire.endTransmission();

  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0xE0);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0xb9);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x80);
  Wire.write(0xA8);
  Wire.write(0x80);
  Wire.write(0x46);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0xD4);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x11);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x35);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0xC8);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x14);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x4C);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x40);
  
  for(int i = 0; i < 69; i++)
  {
    Wire.write(0x00);
  }
  
  Wire.endTransmission();  
  for(int i = 0; i < 68; i++)
  {
    display_byte[i] = 0x00; 
  }
  
  display_default();
  
}

void display::set_left_graph(uint8_t bar, bool pic)	//left bar graph | pic is the outline
{
  byte left_graph = 0b000000;

  left_graph = pow(2, bar) - 1;

  //1st bar
  bitWrite(display_byte[59], 0, bitRead(left_graph, 0));
  //2nd bar
  bitWrite(display_byte[59], 4, bitRead(left_graph, 1));
  //3rd bar
  bitWrite(display_byte[58], 4, bitRead(left_graph, 2));
  //4th bar
  bitWrite(display_byte[57], 0, bitRead(left_graph, 3));
  //5th bar
  bitWrite(display_byte[57], 4, bitRead(left_graph, 4));
  //6th bar
  bitWrite(display_byte[56], 4, bitRead(left_graph, 5));
  //box
  bitWrite(display_byte[55], 4, pic);
}


void display::set_right_graph(uint8_t bar, bool pic)	//right bar graph | pic is the outline
{
  byte right_graph = 0b000000;

  right_graph = pow(2, bar) - 1;

  //1st bar
  bitWrite(display_byte[14], 4, bitRead(right_graph, 0));
  //2nd bar
  bitWrite(display_byte[15], 4, bitRead(right_graph, 1));
  //3rd bar
  bitWrite(display_byte[16], 4, bitRead(right_graph, 2));
  //4th bar
  bitWrite(display_byte[16], 0, bitRead(right_graph, 3));
  //5th bar
  bitWrite(display_byte[17], 4, bitRead(right_graph, 4));
  //6th bar
  bitWrite(display_byte[17], 0, bitRead(right_graph, 5));
  //box
  bitWrite(display_byte[18], 4, pic);
}

void display::set_rc_rssi(uint8_t bar, bool pic)	//bar rssi for tx (upper right) | pic is top right radio + bottom rssi bar
{
  byte rc_rssi = 0b00000;

   rc_rssi = pow(2, bar) - 1;
   
  //1st bar
  bitWrite(display_byte[22], 5, bitRead(rc_rssi, 0));
  //2nd bar
  bitWrite(display_byte[21], 1, bitRead(rc_rssi, 1));
  //3rd bar
  bitWrite(display_byte[21], 3, bitRead(rc_rssi, 2));
  //4th bar
  bitWrite(display_byte[21], 7, bitRead(rc_rssi, 3));
  //5th bar
  bitWrite(display_byte[20], 3, bitRead(rc_rssi, 4));
  //pic
  bitWrite(display_byte[22], 7, pic); //radio
  bitWrite(display_byte[20], 1, pic); //5
  bitWrite(display_byte[21], 5, pic); //4th
  bitWrite(display_byte[21], 6, pic); //3rd
  bitWrite(display_byte[21], 2, pic); //2nd
  bitWrite(display_byte[22], 6, pic); //1st
   
}

void display::set_named_rssi(uint8_t bar, bool pic)	//bar rssi for named (upper left) | pic
{
  byte named_rssi = 0b00000;

  named_rssi = pow(2, bar) - 1;
  
  //1st bar
  bitWrite(display_byte[41], 6, bitRead(named_rssi, 0));
  //2nd bar
  bitWrite(display_byte[41], 5 ,bitRead(named_rssi, 1));
  //3rd bar
  bitWrite(display_byte[41], 7, bitRead(named_rssi, 2));
  //4th bar
  bitWrite(display_byte[40], 3, bitRead(named_rssi, 3));
  //5th bar
  bitWrite(display_byte[40], 7, bitRead(named_rssi, 4));  
  //bottom rssi
  bitWrite(display_byte[41], 2, pic);  //1st
  bitWrite(display_byte[40], 6, pic);
  bitWrite(display_byte[40], 2, pic);
  bitWrite(display_byte[40], 1, pic);
  bitWrite(display_byte[40], 5, pic);
}

void display::set_tx_battery_bar(uint8_t bar, bool pic)	//tx bar battery middle left 
{
  byte tx_battery = 0b000;
  
  tx_battery = pow(2, bar) - 1;
   
  //1st bar
  bitWrite(display_byte[65], 7, bitRead(tx_battery, 0));
  //2nd bar
  bitWrite(display_byte[65], 3 ,bitRead(tx_battery, 1));
  //3rd bar
  bitWrite(display_byte[66], 7, bitRead(tx_battery, 2));
  //box
  bitWrite(display_byte[65], 1, pic);
}

void display::set_tx_battery_percentage(uint8_t percentage, bool pic)  //tx battery percentage middle left
{
  uint8_t tx_battery_percentage_100 = percentage / 100;
  uint8_t tx_battery_percentage_10 = (percentage - tx_battery_percentage_100 * 100 ) / 10;
  uint8_t tx_battery_percentage_1 = (percentage - tx_battery_percentage_100 * 100 - tx_battery_percentage_10 * 10);

  draw_tx_battery_percentage(1, tx_battery_percentage_1);
  draw_tx_battery_percentage(2, tx_battery_percentage_10);
  bitWrite(display_byte[67], 5, tx_battery_percentage_100);
  bitWrite(display_byte[64], 1, pic);
  bitWrite(display_byte[66], 5, pic);
}

void display::draw_tx_battery_percentage(uint8_t display, char digit)  //draw to individual digit of battery percentage
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // tx battery  1
      bitWrite(display_byte[65], 5, bitRead(segment, 0));  //a
      bitWrite(display_byte[64], 0, bitRead(segment, 1));  //b
      bitWrite(display_byte[64], 2, bitRead(segment, 2));  //c
      bitWrite(display_byte[65], 5, bitRead(segment, 3));  //d
      bitWrite(display_byte[65], 2, bitRead(segment, 4));  //e
      bitWrite(display_byte[65], 0, bitRead(segment, 5));  //f
      bitWrite(display_byte[65], 6, bitRead(segment, 6));  //g
      break;
    case 2: // tx battery 10
      bitWrite(display_byte[66], 0, bitRead(segment, 0));  //a
      bitWrite(display_byte[66], 4, bitRead(segment, 1));  //b
      bitWrite(display_byte[66], 6, bitRead(segment, 2));  //c
      bitWrite(display_byte[66], 1, bitRead(segment, 3));  //d
      bitWrite(display_byte[67], 6, bitRead(segment, 4));  //e
      bitWrite(display_byte[67], 4, bitRead(segment, 5));  //f
      bitWrite(display_byte[66], 2, bitRead(segment, 6));  //g
      break;
  }
}

void display::set_drone_battery_bar(uint8_t bar, bool pic)  //drone bar battery middle top 
{
  byte drone_battery = 0b000;

  drone_battery = pow(2, bar) - 1;

  //1st bar
  bitWrite(display_byte[31], 6, bitRead(drone_battery, 0));
  //2nd bar
  bitWrite(display_byte[32], 2 ,bitRead(drone_battery, 1));
  //3rd bar
  bitWrite(display_byte[32], 1, bitRead(drone_battery, 2));

  bitWrite(display_byte[32], 3, pic);
}

void display::set_drone_battery_percentage(uint8_t percentage, bool pic)  //drone battery percentage middle top
{
  uint8_t drone_battery_percentage_hundred = percentage / 100;
  uint8_t drone_battery_percentage_10 = (percentage - drone_battery_percentage_hundred * 100 ) / 10;
  uint8_t drone_battery_percentage_units= (percentage - drone_battery_percentage_hundred * 100 - drone_battery_percentage_10 * 10);

  draw_drone_battery_percentage(1, drone_battery_percentage_units);
  draw_drone_battery_percentage(2, drone_battery_percentage_10);
  bitWrite(display_byte[32], 6, drone_battery_percentage_hundred);
  
  bitWrite(display_byte[29], 2, pic);
}

void display::draw_drone_battery_percentage(uint8_t display, char digit)  //draw to individual digit of drone percentage
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // drone battery  1
      bitWrite(display_byte[30], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[29], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[29], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[30], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[30], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[30], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[30], 5, bitRead(segment, 6));  //g
      break;
    case 2: // drone battery 10
      bitWrite(display_byte[31], 3, bitRead(segment, 0));  //a
      bitWrite(display_byte[31], 7, bitRead(segment, 1));  //b
      bitWrite(display_byte[31], 5, bitRead(segment, 2));  //c
      bitWrite(display_byte[31], 2, bitRead(segment, 3));  //d
      bitWrite(display_byte[32], 5, bitRead(segment, 4));  //e
      bitWrite(display_byte[32], 7, bitRead(segment, 5));  //f
      bitWrite(display_byte[31], 1, bitRead(segment, 6));  //g
      break;
  }
}

void display::set_rpm(int rpm, bool pic)  //set rpm value on upper right corner. if value > 1999 than x10 is on. value above 19990 wont work
{
  bool rpm10 = 0;
  
  if(rpm > 1999)
  {
    rpm10 = 1;
    rpm = rpm / 10; 
  }

  int rpm_units = 0;
  int rpm_10 = 0;
  int rpm_100 = 0;
  int rpm_1000 = 0;

  rpm_1000 = rpm % 10000 / 1000;
  rpm_100  = rpm % 1000 / 100;
  rpm_10      = rpm % 100 / 10;
  rpm_units     = rpm % 10;

  draw_rpm(1, rpm_units);
  draw_rpm(2, rpm_10);
  draw_rpm(3, rpm_100);
  bitWrite(display_byte[17], 2, rpm_1000);
  bitWrite(display_byte[16], 2, rpm10);
  bitWrite(display_byte[16], 6, rpm10);
  bitWrite(display_byte[15], 6, pic);
  
}

void display::draw_rpm(uint8_t display, char digit)  //draw to individual digit of rpm
{

  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: //rpm 1
      bitWrite(display_byte[14], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[13], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[13], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[14], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[14], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[14], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[14], 5, bitRead(segment, 6));  //g
      break;
    case 2: // rpm 10
      bitWrite(display_byte[15], 3, bitRead(segment, 0));  //a
      bitWrite(display_byte[15], 7, bitRead(segment, 1));  //b
      bitWrite(display_byte[15], 5, bitRead(segment, 2));  //c
      bitWrite(display_byte[15], 2, bitRead(segment, 3));  //d
      bitWrite(display_byte[16], 5, bitRead(segment, 4));  //e
      bitWrite(display_byte[16], 7, bitRead(segment, 5));  //f
      bitWrite(display_byte[15], 1, bitRead(segment, 6));  //g
      break;
    case 3: // rpm 100
      bitWrite(display_byte[17], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[16], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[16], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[17], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[17], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[17], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[17], 5, bitRead(segment, 6));  //g
      break;
  }
}

void display::set_speed(int speed, bool pic)	//set speed value, input 999 output 99.9, input 1000 output 100 | pic is kmh no support for mph yet
{

  int speed_1   = 0;
  int speed_10  = 0;
  int speed_100 = 0;
  int speed_1000 = 0;
  bool dot = 0;

  speed_1000 = speed % 10000 / 1000;
  speed_100  = speed % 1000 / 100;
  speed_10   = speed % 100 / 10;
  speed_1    = speed % 10;

  if(speed > 999)
  {
    dot = 0;
    draw_speed(1, speed_10);
    draw_speed(2, speed_100);
    draw_speed(3, speed_1000);
  }
  else
  {
    dot = 1;
    draw_speed(1, speed_1);
    draw_speed(2, speed_10);
    draw_speed(3, speed_100);
  }
  bitWrite(display_byte[58], 6, pic); //kmh
  bitWrite(display_byte[57], 6, dot);
}

void display::draw_speed(uint8_t display, char digit)  //draw to individual digit of speed
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // speed 1
      bitWrite(display_byte[56], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[55], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[55], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[56], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[56], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[56], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[56], 5, bitRead(segment, 6));  //g
      break;
    case 2: // speed 10
      bitWrite(display_byte[57], 3, bitRead(segment, 0));  //a
      bitWrite(display_byte[57], 7, bitRead(segment, 1));  //b
      bitWrite(display_byte[57], 5, bitRead(segment, 2));  //c
      bitWrite(display_byte[57], 2, bitRead(segment, 3));  //d
      bitWrite(display_byte[58], 5, bitRead(segment, 4));  //e
      bitWrite(display_byte[58], 7, bitRead(segment, 5));  //f
      bitWrite(display_byte[57], 1, bitRead(segment, 6));  //g
      break;
    case 3: // speed 100
      bitWrite(display_byte[59], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[58], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[58], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[59], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[59], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[59], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[59], 5, bitRead(segment, 6));  //g
      break;
  }
}

void display::set_distance(int distance, bool pic)	//set distance, 5 digit max | pic is distance on bottom right | only meter yet (turned on by pic)
{
  int distance_1 = 0;
  int distance_10 = 0;
  int distance_100 = 0;
  int distance_1000 = 0;
  int distance_10000 = 0;

  distance_10000 = distance % 100000 / 10000;
  distance_1000  = distance % 10000 / 1000;
  distance_100   = distance % 1000 / 100;
  distance_10    = distance % 100 / 10;
  distance_1     = distance % 10;

  draw_distance(1, distance_1);
  draw_distance(2, distance_10);
  draw_distance(3, distance_100);
  draw_distance(4, distance_1000);
  draw_distance(5, distance_10000);
  bitWrite(display_byte[3], 4, pic);
  bitWrite(display_byte[0], 1, pic);
}

void display::draw_distance(uint8_t display, char digit)  //draw to individual digit of distance
{
  byte segment = char_to_7_segment(digit);
 
  bitWrite(display_byte[display], 3, bitRead(segment, 0));  //a
  bitWrite(display_byte[display], 7, bitRead(segment, 1));  //b
  bitWrite(display_byte[display], 6, bitRead(segment, 2));  //c
  bitWrite(display_byte[display], 0, bitRead(segment, 3));  //d
  bitWrite(display_byte[display], 2, bitRead(segment, 4));  //e
  bitWrite(display_byte[display], 1, bitRead(segment, 5));  //f
  bitWrite(display_byte[display], 5, bitRead(segment, 6));  //g
}

void display::set_altitude(int altitude, bool pic)	//set altitude, 4 digit max | pic is altitude on middle bottom | only meter yet (turn on by pic)
{
  int altitude_1 = 0;
  int altitude_10 = 0;
  int altitude_100 = 0;
  int altitude_1000 = 0;
  bool negative = 0;
  
  if(altitude < 0)
  {
     negative = 1;
  }
  
  altitude_1000  = altitude % 10000 / 1000;
  altitude_100   = altitude % 1000 / 100;
  altitude_10    = altitude % 100 / 10;
  altitude_1     = altitude % 10;
	
  draw_altitude(1, altitude_1);
  draw_altitude(2, altitude_10);
  draw_altitude(3, altitude_100);
  draw_altitude(4, altitude_1000);
  
  bitWrite(display_byte[8], 4, pic); //height
  bitWrite(display_byte[6], 1, pic); //meter
  bitWrite(display_byte[9], 4, negative);
}

void display::draw_altitude(uint8_t display, char digit)  //draw to individual digit of altitude
{
  byte segment = char_to_7_segment(digit);
 
  bitWrite(display_byte[display + 6], 3, bitRead(segment, 0));  //a
  bitWrite(display_byte[display + 6], 7, bitRead(segment, 1));  //b
  bitWrite(display_byte[display + 6], 6, bitRead(segment, 2));  //c
  bitWrite(display_byte[display + 6], 0, bitRead(segment, 3));  //d
  bitWrite(display_byte[display + 6], 2, bitRead(segment, 4));  //e
  bitWrite(display_byte[display + 6], 1, bitRead(segment, 5));  //f
  bitWrite(display_byte[display + 6], 5, bitRead(segment, 6));  //g
}

void display::set_clearance(int clearance, bool pic)		//set clearance, 3 digit max, input 123 output 12, input 12 output 1.2 | pic is clearance, meter, triangle, saw teeth
{
  int clearance_1 = 0;
  int clearance_10 = 0;
  int clearance_100 = 0;

  clearance_100   = clearance % 1000 / 100;
  clearance_10    = clearance % 100 / 10;
  clearance_1     = clearance % 10;
  
  if(clearance_100)
  {
    draw_clearance(1, clearance_10);
    draw_clearance(2, clearance_100);
    bitWrite(display_byte[12] , 0, 0);
  }
  else
  {
    draw_clearance(1, clearance_1);
    draw_clearance(2, clearance_10);    
    bitWrite(display_byte[12] , 0, 1);
  }
  
  bitWrite(display_byte[11], 6, pic);  //clearance
  bitWrite(display_byte[11], 5, pic);  //meter
  bitWrite(display_byte[11], 0, pic);  //triangle
  bitWrite(display_byte[11], 4, pic);  //saw teeth
}

void display::draw_clearance(uint8_t display, char digit)  //draw to individual digit of clearance
{
  byte segment = char_to_7_segment(digit);
 
  bitWrite(display_byte[display + 11], 7, bitRead(segment, 0));  //a
  bitWrite(display_byte[display + 10], 3, bitRead(segment, 1));  //b
  bitWrite(display_byte[display + 10], 2, bitRead(segment, 2));  //c
  bitWrite(display_byte[display + 11], 4, bitRead(segment, 3));  //d
  bitWrite(display_byte[display + 11], 6, bitRead(segment, 4));  //e
  bitWrite(display_byte[display + 11], 5, bitRead(segment, 5));  //f
  bitWrite(display_byte[display + 10], 1, bitRead(segment, 6));  //g

}

void display::set_ev(int ev, bool pic)  //set ev, 3 digit max, 2 if using + or - | pic is ev | dot kinda supported
{
  int ev_1   = 0;
  int ev_10  = 0;
  int ev_100 = 0;

  ev_100 = ev % 1000 / 100;
  ev_10  = ev % 100 / 10;
  ev_1   = ev % 10;
  
  bitWrite(display_byte[61], 1, 0);  //dot
  bitWrite(display_byte[63], 0, 0);  //+1
  bitWrite(display_byte[63], 1, 0);  //+2
  bitWrite(display_byte[63], 2, 0);  //-
  draw_ev(1, ev_1);
  draw_ev(2, ev_10);
  draw_ev(3, ev_100);
  bitWrite(display_byte[60], 5, pic);
}

void display::draw_ev(uint8_t display, char digit) //draw to individual digit of ev 
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // ev 1
      bitWrite(display_byte[60], 0, bitRead(segment, 0));  //a
      bitWrite(display_byte[60], 4, bitRead(segment, 1));  //b
      bitWrite(display_byte[60], 6, bitRead(segment, 2));  //c
      bitWrite(display_byte[60], 1, bitRead(segment, 3));  //d
      bitWrite(display_byte[61], 6, bitRead(segment, 4));  //e
      bitWrite(display_byte[61], 4, bitRead(segment, 5));  //f
      bitWrite(display_byte[60], 2, bitRead(segment, 6));  //g
      break;
    case 2: // ev 10
      bitWrite(display_byte[62], 4, bitRead(segment, 0));  //a
      bitWrite(display_byte[61], 0, bitRead(segment, 1));  //b
      bitWrite(display_byte[61], 2, bitRead(segment, 2));  //c
      bitWrite(display_byte[62], 5, bitRead(segment, 3));  //d
      bitWrite(display_byte[62], 2, bitRead(segment, 4));  //e
      bitWrite(display_byte[62], 0, bitRead(segment, 5));  //f
      bitWrite(display_byte[62], 6, bitRead(segment, 6));  //g
      break;
    case 3: // ev 100
      bitWrite(display_byte[64], 4, bitRead(segment, 0));  //a
      bitWrite(display_byte[63], 4, bitRead(segment, 1));  //b
      bitWrite(display_byte[63], 6, bitRead(segment, 2));  //c
      bitWrite(display_byte[63], 5, bitRead(segment, 3));  //d
      bitWrite(display_byte[64], 5, bitRead(segment, 4));  //e
      bitWrite(display_byte[64], 6, bitRead(segment, 5));  //f
      bitWrite(display_byte[63], 2, bitRead(segment, 6));  //g
      break;
  }
}

void display::set_sd(bool sd)	//turn on sd logo mid right
{
  bitWrite(display_byte[60], 7, sd);
}

void display::set_sport(bool sport)  //turn on sport logo full right
{
  bitWrite(display_byte[60], 3, sport);
}

void display::set_vision(bool vision)  //turn on vision mid left
{
  bitWrite(display_byte[64], 3, vision);
}

void display::set_rec(bool rec)  //turn on rec 
{
  bitWrite(display_byte[63], 7, rec);
}

void display::set_text(String text, int scroll_speed)  //set text to display on the 11 14 segment display. if size of text > 11 the text will scroll automaticly | scroll speed is in ms 
{
  if(text.length() > 11)
  {
    for(int j = 0; text.length() - j + 2 > 11; j++)
    {
      for(int i = 0; i < text.length() ; i++)
      {
        draw_text(i - j, text.charAt(i)); 
      }
      update_display();
      delay(scroll_speed);
    }
  }
  else
  {
    for(int i = 0; i < text.length() ; i++)
    {
      draw_text(i, text.charAt(i)); 
    }
    if(text.length() < 11)
    {
      for(int i = 0; i < 11 - text.length(); i++)
      {
        draw_text(i + text.length(), 32);
      }
    }
  }
}

void display::update_display()	//update display, has to be called to update display.
{
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x38);
  Wire.write(0x00);
  Wire.write(0x4C);
  Wire.endTransmission();

  Wire.beginTransmission(0x38);
  Wire.write(0x40);
    
  for(int i = 0; i < 68; i++)
  {
    Wire.write(display_byte[i]);
  }
  
  Wire.endTransmission();  
}

void display::draw_text(int display, int chara)	//draw to individual digit of 14 segment displays
{
  int segment = char_to_14_segment(chara);
  switch(display)
  {
  case 0:
    bitWrite(display_byte[53], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[53], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[53], 6, bitRead(segment, 2));  //c
    bitWrite(display_byte[54], 4, bitRead(segment, 3));  //d
    bitWrite(display_byte[54], 2, bitRead(segment, 4));  //e
    bitWrite(display_byte[54], 3, bitRead(segment, 5));  //f
    bitWrite(display_byte[54], 1, bitRead(segment, 6));  //g
    bitWrite(display_byte[53], 2, bitRead(segment, 7));  //h
    bitWrite(display_byte[53], 0, bitRead(segment, 8));  //i
    bitWrite(display_byte[54], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[54], 0, bitRead(segment, 10));  //k
    bitWrite(display_byte[54], 7, bitRead(segment, 11));  //l
    bitWrite(display_byte[54], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[53], 1, bitRead(segment, 13));  //n
    break; 
  case 1:
    bitWrite(display_byte[52], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[51], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[51], 2, bitRead(segment, 2));  //c
    bitWrite(display_byte[52], 4, bitRead(segment, 3));  //d
    bitWrite(display_byte[53], 4, bitRead(segment, 4));  //e
    bitWrite(display_byte[53], 5, bitRead(segment, 5));  //f
    bitWrite(display_byte[52], 2, bitRead(segment, 6));  //g
    bitWrite(display_byte[51], 1, bitRead(segment, 7));  //h
    bitWrite(display_byte[51], 0, bitRead(segment, 8));  //i
    bitWrite(display_byte[52], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[52], 0, bitRead(segment, 10));  //k
    bitWrite(display_byte[52], 1, bitRead(segment, 11));  //l
    bitWrite(display_byte[52], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[52], 7, bitRead(segment, 13));  //n
    break; 
  case 2:
    bitWrite(display_byte[38], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[38], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[38], 6, bitRead(segment, 2));  //c
    bitWrite(display_byte[39], 4, bitRead(segment, 3));  //d
    bitWrite(display_byte[39], 2, bitRead(segment, 4));  //e
    bitWrite(display_byte[39], 3, bitRead(segment, 5));  //f
    bitWrite(display_byte[39], 1, bitRead(segment, 6));  //g
    bitWrite(display_byte[38], 2, bitRead(segment, 7));  //h
    bitWrite(display_byte[38], 0, bitRead(segment, 8));  //i
    bitWrite(display_byte[39], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[39], 0, bitRead(segment, 10));  //k
    bitWrite(display_byte[39], 7, bitRead(segment, 11));  //l
    bitWrite(display_byte[39], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[38], 1, bitRead(segment, 13));  //n
    break; 
  case 3:
    bitWrite(display_byte[37], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[36], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[36], 2, bitRead(segment, 2));  //c
    bitWrite(display_byte[37], 4, bitRead(segment, 3));  //d
    bitWrite(display_byte[38], 4, bitRead(segment, 4));  //e
    bitWrite(display_byte[38], 5, bitRead(segment, 5));  //f
    bitWrite(display_byte[37], 2, bitRead(segment, 6));  //g
    bitWrite(display_byte[36], 1, bitRead(segment, 7));  //h
    bitWrite(display_byte[36], 0, bitRead(segment, 8));  //i
    bitWrite(display_byte[37], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[37], 0, bitRead(segment, 10));  //k
    bitWrite(display_byte[37], 1, bitRead(segment, 11));  //l
    bitWrite(display_byte[37], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[37], 7, bitRead(segment, 13));  //n
    break;
  case 4:
    bitWrite(display_byte[35], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[34], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[34], 2, bitRead(segment, 2));  //c
    bitWrite(display_byte[35], 0, bitRead(segment, 3));  //d
    bitWrite(display_byte[36], 6, bitRead(segment, 4));  //e
    bitWrite(display_byte[36], 7, bitRead(segment, 5));  //f
    bitWrite(display_byte[36], 5, bitRead(segment, 6));  //g
    bitWrite(display_byte[35], 6, bitRead(segment, 7));  //h
    bitWrite(display_byte[35], 4, bitRead(segment, 8));  //i
    bitWrite(display_byte[35], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[36], 4, bitRead(segment, 10));  //k
    bitWrite(display_byte[35], 3, bitRead(segment, 11));  //l
    bitWrite(display_byte[35], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[35], 5, bitRead(segment, 13));  //n
    break;  
  case 5:
    bitWrite(display_byte[34], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[33], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[33], 6, bitRead(segment, 2));  //c
    bitWrite(display_byte[33], 0, bitRead(segment, 3));  //d
    bitWrite(display_byte[34], 0, bitRead(segment, 4));  //e
    bitWrite(display_byte[34], 1, bitRead(segment, 5));  //f
    bitWrite(display_byte[34], 6, bitRead(segment, 6));  //g
    bitWrite(display_byte[33], 5, bitRead(segment, 7));  //h
    bitWrite(display_byte[33], 4, bitRead(segment, 8));  //i
    bitWrite(display_byte[33], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[34], 4, bitRead(segment, 10));  //k
    bitWrite(display_byte[34], 5, bitRead(segment, 11));  //l
    bitWrite(display_byte[33], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[33], 3, bitRead(segment, 13));  //n
    break;
  case 6:
    bitWrite(display_byte[28], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[27], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[27], 2, bitRead(segment, 2));  //c
    bitWrite(display_byte[28], 0, bitRead(segment, 3));  //d
    bitWrite(display_byte[29], 6, bitRead(segment, 4));  //e
    bitWrite(display_byte[29], 7, bitRead(segment, 5));  //f
    bitWrite(display_byte[29], 5, bitRead(segment, 6));  //g
    bitWrite(display_byte[28], 6, bitRead(segment, 7));  //h
    bitWrite(display_byte[28], 4, bitRead(segment, 8));  //i
    bitWrite(display_byte[28], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[29], 4, bitRead(segment, 10));  //k
    bitWrite(display_byte[28], 3, bitRead(segment, 11));  //l
    bitWrite(display_byte[28], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[28], 5, bitRead(segment, 13));  //n
    break;
  case 7:
    bitWrite(display_byte[27], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[26], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[26], 6, bitRead(segment, 2));  //c
    bitWrite(display_byte[26], 0, bitRead(segment, 3));  //d
    bitWrite(display_byte[27], 0, bitRead(segment, 4));  //e
    bitWrite(display_byte[27], 1, bitRead(segment, 5));  //f
    bitWrite(display_byte[27], 6, bitRead(segment, 6));  //g
    bitWrite(display_byte[26], 5, bitRead(segment, 7));  //h
    bitWrite(display_byte[26], 4, bitRead(segment, 8));  //i
    bitWrite(display_byte[26], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[27], 4, bitRead(segment, 10));  //k
    bitWrite(display_byte[27], 5, bitRead(segment, 11));  //l
    bitWrite(display_byte[26], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[26], 3, bitRead(segment, 13));  //n
    break;
  case 8:
    bitWrite(display_byte[24], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[24], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[24], 6, bitRead(segment, 2));  //c
    bitWrite(display_byte[25], 4, bitRead(segment, 3));  //d
    bitWrite(display_byte[25], 2, bitRead(segment, 4));  //e
    bitWrite(display_byte[25], 3, bitRead(segment, 5));  //f
    bitWrite(display_byte[25], 1, bitRead(segment, 6));  //g
    bitWrite(display_byte[24], 2, bitRead(segment, 7));  //h
    bitWrite(display_byte[24], 0, bitRead(segment, 8));  //i
    bitWrite(display_byte[25], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[25], 0, bitRead(segment, 10));  //k
    bitWrite(display_byte[25], 7, bitRead(segment, 11));  //l
    bitWrite(display_byte[25], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[24], 1, bitRead(segment, 13));  //n
    break;   
  case 9:
    bitWrite(display_byte[23], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[22], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[22], 2, bitRead(segment, 2));  //c
    bitWrite(display_byte[23], 4, bitRead(segment, 3));  //d
    bitWrite(display_byte[24], 4, bitRead(segment, 4));  //e
    bitWrite(display_byte[24], 5, bitRead(segment, 5));  //f
    bitWrite(display_byte[23], 2, bitRead(segment, 6));  //g
    bitWrite(display_byte[22], 1, bitRead(segment, 7));  //h
    bitWrite(display_byte[22], 0, bitRead(segment, 8));  //i
    bitWrite(display_byte[23], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[23], 0, bitRead(segment, 10));  //k
    bitWrite(display_byte[23], 1, bitRead(segment, 11));  //l
    bitWrite(display_byte[23], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[23], 7, bitRead(segment, 13));  //n
    break;  
  case 10:
    bitWrite(display_byte[19], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[18], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[18], 2, bitRead(segment, 2));  //c
    bitWrite(display_byte[19], 0, bitRead(segment, 3));  //d
    bitWrite(display_byte[20], 6, bitRead(segment, 4));  //e
    bitWrite(display_byte[20], 7, bitRead(segment, 5));  //f
    bitWrite(display_byte[20], 5, bitRead(segment, 6));  //g
    bitWrite(display_byte[19], 6, bitRead(segment, 7));  //h
    bitWrite(display_byte[19], 4, bitRead(segment, 8));  //i
    bitWrite(display_byte[19], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[20], 4, bitRead(segment, 10));  //k
    bitWrite(display_byte[19], 3, bitRead(segment, 11));  //l
    bitWrite(display_byte[19], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[19], 5, bitRead(segment, 13));  //n
    break;
    
  }
}

void display::set_name(String name, int scroll_speed)  //set text for rssi name 4 14 segment display. if size of text > 4 the text will scroll automaticly | scroll speed is in ms 
{
  if(name.length() > 4)
  {
    for(int j = 0; name.length() - j + 2 > 4; j++)
    {
      for(int i = 0; i < name.length() ; i++)
      {
        draw_name(i - j, name.charAt(i)); 
      }
      update_display();
      delay(scroll_speed);
    }
  }
  else
  {
    for(int i = 0; i < name.length() ; i++)
    {
      draw_name(i, name.charAt(i)); 
    }
    if(name.length() < 4)
    {
      for(int i = 0; i < 4 - name.length(); i++)
      {
        draw_name(i + name.length(), 32); //fill with space if less than 4 char
      }
    }
  }
}

void display::draw_name(int display, int chara)	//draw to individual digit of 14 segment displays
{
  int segment = char_to_14_segment(chara);
  switch(display)
  {
  case 0:
    bitWrite(display_byte[50], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[49], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[49], 5, bitRead(segment, 2));  //c
    bitWrite(display_byte[51], 6, bitRead(segment, 3));  //d
    bitWrite(display_byte[51], 5, bitRead(segment, 4));  //e
    bitWrite(display_byte[51], 7, bitRead(segment, 5));  //f
    bitWrite(display_byte[50], 1, bitRead(segment, 6));  //g
    bitWrite(display_byte[49], 1, bitRead(segment, 7));  //h
    bitWrite(display_byte[49], 2, bitRead(segment, 8));  //i
    bitWrite(display_byte[50], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[50], 2, bitRead(segment, 10));  //k
    bitWrite(display_byte[50], 3, bitRead(segment, 11));  //l
    bitWrite(display_byte[50], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[49], 3, bitRead(segment, 13));  //n
    break; 
  case 1:
    bitWrite(display_byte[47], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[46], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[46], 1, bitRead(segment, 2));  //c
    bitWrite(display_byte[48], 2, bitRead(segment, 3));  //d
    bitWrite(display_byte[48], 1, bitRead(segment, 4));  //e
    bitWrite(display_byte[48], 3, bitRead(segment, 5));  //f
    bitWrite(display_byte[48], 5, bitRead(segment, 6));  //g
    bitWrite(display_byte[47], 5, bitRead(segment, 7));  //h
    bitWrite(display_byte[47], 6, bitRead(segment, 8));  //i
    bitWrite(display_byte[47], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[48], 6, bitRead(segment, 10));  //k
    bitWrite(display_byte[48], 7, bitRead(segment, 11));  //l
    bitWrite(display_byte[47], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[47], 7, bitRead(segment, 13));  //n
    break;
  case 2:
    bitWrite(display_byte[45], 7, bitRead(segment, 0));  //a
    bitWrite(display_byte[44], 7, bitRead(segment, 1));  //b
    bitWrite(display_byte[44], 5, bitRead(segment, 2));  //c
    bitWrite(display_byte[46], 6, bitRead(segment, 3));  //d
    bitWrite(display_byte[46], 5, bitRead(segment, 4));  //e
    bitWrite(display_byte[46], 7, bitRead(segment, 5));  //f
    bitWrite(display_byte[45], 1, bitRead(segment, 6));  //g
    bitWrite(display_byte[44], 1, bitRead(segment, 7));  //h
    bitWrite(display_byte[44], 2, bitRead(segment, 8));  //i
    bitWrite(display_byte[45], 6, bitRead(segment, 9));  //j
    bitWrite(display_byte[45], 2, bitRead(segment, 10));  //k
    bitWrite(display_byte[45], 3, bitRead(segment, 11));  //l
    bitWrite(display_byte[45], 5, bitRead(segment, 12));  //m
    bitWrite(display_byte[44], 3, bitRead(segment, 13));  //n
    break;
  case 3:
    bitWrite(display_byte[42], 3, bitRead(segment, 0));  //a
    bitWrite(display_byte[41], 3, bitRead(segment, 1));  //b
    bitWrite(display_byte[41], 1, bitRead(segment, 2));  //c
    bitWrite(display_byte[43], 2, bitRead(segment, 3));  //d
    bitWrite(display_byte[43], 1, bitRead(segment, 4));  //e
    bitWrite(display_byte[43], 3, bitRead(segment, 5));  //f
    bitWrite(display_byte[43], 5, bitRead(segment, 6));  //g
    bitWrite(display_byte[42], 5, bitRead(segment, 7));  //h
    bitWrite(display_byte[42], 6, bitRead(segment, 8));  //i
    bitWrite(display_byte[42], 2, bitRead(segment, 9));  //j
    bitWrite(display_byte[43], 6, bitRead(segment, 10));  //k
    bitWrite(display_byte[43], 7, bitRead(segment, 11));  //l
    bitWrite(display_byte[42], 1, bitRead(segment, 12));  //m
    bitWrite(display_byte[42], 7, bitRead(segment, 13));  //n
    break;
  }
}

byte display::char_to_7_segment(char digit) //input number, output segment to turn on. order is 0bgfedcba
{
  byte bit_7_digit[] = {63,6,91,79,102,109,125,7,127,111};

  return bit_7_digit[digit];
}

int display::char_to_14_segment(int digit)  //input char, output segment to turn on. order is 0bnmlkjihgfedcba
{
  if(digit == 32)
  {
    digit = 26 + 97;
  }
  if(47 < digit && digit < 58)
  {
    digit = 27 + 97 - 48 + digit; 
  }
  digit = digit - 97;

  int bit_14_digit[] = {
                         247,  //A   0
                         4751,  //B   1
                         57,  //C   2
                         4623,  //D   3
                         121,  //E   4
                         113,  //F   5
                         189,  //G   6
                         246,  //H   7
                         4617,  //I   8
                         1038,  //J   9
                         8560,  //K   10
                         56,  //L   11
                         10294,  //M   12
                         2358,  //N   13
                         63,  //O   14
                         243,  //P   15
                         319,  //Q   16
                         499,  //R   17
                         237,  //S   18
                         4609,  //T   19
                         62,  //U   20
                         738,  //V   21
                         1334,  //W   22
                         11520,  //X   23
                         10752,  //Y   24
                         9225,  //Z   25
                         0,   //space 26
                         63,  //0
                         6, //1
                         219, //2
                         143, //3
                         230,//4
                         237,//5
                         253,//6
                         7,//7
                         255,//8
                         239//9
                        };

  
  return bit_14_digit[digit];
}

void display::display_default()  //display 0 everywhere
{
  display_byte[0] =  0b00000010;  // bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: xxx | bit 5: feet on bottom right | bit 6: xxx | bit 7: meters on bottom right | bit 8: xxx  
  display_byte[1] =  0b11001111;  // bit 1: b 5th dist | bit 2: c 5th dist | bit 3: g 5th dist | bit 4: xxx | bit 5: a 5th dist | bit 6: e 5th dist | bit 7: f 5th dist | bit 8: d 5th dist
  display_byte[2] =  0b11001111;  // bit 1: b 4th dist | bit 2: c 4th dist | bit 3: g 4th dist | bit 4: xxx | bit 5: a 4th dist | bit 6: e 4th dist | bit 7: f 4th dist | bit 8: d 4th dist
  display_byte[3] =  0b11011111;  // bit 1: b 3rd dist | bit 2: c 3rd dist | bit 3: g 3rd dist | bit 4: dist| bit 5: a 3rd dist | bit 6: e 3rd dist | bit 7: f 3rd dist | bit 8: d 3rd dist
  display_byte[4] =  0b11001111;  // bit 1: b 2nd dist | bit 2: c 2nd dist | bit 3: g 2nd dist | bit 4: bottom line | bit 5: a 2nd dist | bit 6: e 2nd dist | bit 7: f 2nd dist | bit 8: d 2nd dist
  display_byte[5] =  0b11001111;  // bit 1: b 1st dist | bit 2: c 1st dist | bit 3: g 1st dist | bit 4: 3rd bottom arrow | bit 5: a 1st dist | bit 6: e 1st dist | bit 7: f 1st dist | bit 8: d 1st dist
  display_byte[6] =  0b00000010;  // 0b1234f6m8 | 1st bit is t line | 2nd bit is 2nd arrow up | 3rd bit is 3rd arrow up | 4th bit is 1st arrow down or 2nd arrow down| 6th bit is 1st arrow up | 8th bit 
  display_byte[7] =  0b11001111;  // bit 1: b 4th alti | bit 2: c 4th alti | bit 3: g 4th alti | bit 4: xxx | bit 5: a 4th alti | bit 6: e 4th alti | bit 7: f 4th alti | bit 8: d 4th alti 
  display_byte[8] =  0b11011111;  // bit 1: b 3rd alti | bit 2: c 3rd alti | bit 3: g 3rd alti | bit 4: height | bit 5: a 3rd alti | bit 6: e 3rd alti | bit 7: f 3rd alti | bit 8: d 3rd alti 
  display_byte[9] =  0b11001111;  // bit 1: b 2nd alti | bit 2: c 2nd alti | bit 3: g 2nd alti | bit 4: negative height | bit 5: a 2nd alti | bit 6: e 2nd alti | bit 7: f 2nd alti | bit 8: d 2nd alti 
  display_byte[10] = 0b11011111;  // bit 1: b 1st alti | bit 2: c 1st alti | bit 3: g 1st alti | bit 4: xxx | bit 5: a 1st alti | bit 6: e 1st alti | bit 7: f 1st alti | bit 8: d 1st alti 
  display_byte[11] = 0b01111101;  // bit 1: ft on bottom left | bit 2: clearance | bit 3: meter on bottom left | bit 4: saw teeth on bottom left | bit 5: b 1clear | bit 6: c 1clear | bit 7: g 1clear|  bit 8 is triangle on bottom left
  display_byte[12] = 0b11111101;  // bit 1: a 1clear | bit 2: e 1clear | bit 3: f 1clear | bit 4: d 1clear | bit 5: b 10 clear | bit 6: c 10 clear | bit 7: g 10 clear | bit 8: dot clear
  display_byte[13] = 0b11111010;  // bit 1: a 10 clear | bit 2: e 10 clear | bit 3: f 10 clear | bit 4: d 10 clear | bit 5: b 1rpm | bit 6: xxx | bit 7: c 1rpm | bit 8: xxx
  display_byte[14] = 0b11001010;  // bit 1: a 1rpm | bit 2: d 1rpm | bit 3: g 1rpm | bit 4: 1st bar r graph | bit 5: f 1rpm | bit 6: xxx | bit 7: e 1rpm | bit 8: xxx 
  display_byte[15] = 0b11101101;  // bit 1: b 10 rpm | bit 2: RPM | bit 3: c 10 rpm | bit 4: 2nd bar r graph | bit 5: a 10 rpm | bit 6: d 10 rpm | bit 7: g 10 rpm | bit 8: xxx
  display_byte[16] = 0b10101010;  // bit 1: f 10 rpm | bit 2: 10 rpm | bit 3: e 10 rpm | bit 4: 3rd bar r graph | bit 5: b 100 rpm | bit 6: *rpm | bit 7: c 100 rpm | bit 8: 4th bar r graph 
  display_byte[17] = 0b11001110;  // bit 1: a 100 rpm | bit 2: d 100 rpm | bit 3: g 100 rpm | bit 4: 5th bar r graph | bit 5: f 100 rpm | bit 6: thousand rpm | bit 7: e 100 rpm | bit 8: 6th bar r graph
  display_byte[18] = 0b00011100;  // bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: right graph box | bit 5: b 11th  | bit 6: c 11th  | bit 7: xxx | bit 8: xxx
  display_byte[19] = 0b10000001;  // bit 1: a 11th | bit 2: h 11th | bit 3: n 11th | bit 4: i 11th | bit 5: l 11th | bit 6: j 11th | bit 7: m 11th | bit 8: d 11th 
  display_byte[20] = 0b11000010;  // bit 1: f 11th | bit 2: e 11th | bit 3: g 11th | bit 4: k 11th | bit 5: t 5th radio rssi | bit 6: xxx | bit 7: b 5th radio rssi | bit 8: xxx
  display_byte[21] = 0b01100100;  // bit 1: t 4th radio rssi | bit 2: b 3rd radio rssi | bit 3: b 4th radio rssi | bit 4: xxx | bit 5: t 3rd radio rssi | bit 6: b 2nd radio rssi | bit 7: t 2nd radio rssi | bit 8: xxx
  display_byte[22] = 0b11001100;  // bit 1: radio | bit 2: b 1st radio rssi | bit 3: t 1st radio rssi | bit 4: xxx | bit 5: b 10th | bit 6: c 10th | bit 7: h 10th | bit 8: i 10th 
  display_byte[23] = 0b00011000;  // bit 1: n 10th | bit 2: j 10th | bit 3: m 10th | bit 4: d 10th | bit 5: a 10th | bit 6: g 10th | bit 7: l 10th | bit 8: k 10th 
  display_byte[24] = 0b11111000;  // bit 1: b 9th  | bit 2: c 9th  | bit 3: f 9th  | bit 4: e 10th  | bit 5: a 9th  | bit 6: h 9th  | bit 7: n 9th  | bit 8: i 9th 
  display_byte[25] = 0b00011100;  // bit 1: l 9th  | bit 2: j 9th  | bit 3: m 9th  | bit 4: d 9th  | bit 5: f 9th  | bit 6: e 9th  | bit 7: g 9th  | bit 8: k 9th 
  display_byte[26] = 0b11000001;  // bit 1: b 8th  | bit 2: c 8th  | bit 3: h 8th  | bit 4: c 8th  | bit 5: n 8th  | bit 6: j 8th  | bit 7: m 8th  | bit 8: d 8th 
  display_byte[27] = 0b10001111;  // bit 1: a 8th  | bit 2: g 8th  | bit 3: l 8th  | bit 4: k 8th  | bit 5: b 7th  | bit 6: c 7th  | bit 7: f 8th | bit 8: e 8th
  display_byte[28] = 0b10000001;  // bit 1: a 7th  | bit 2: h 7th  | bit 3: n 7th  | bit 4: i 7th  | bit 5: l 7th  | bit 6: j 7th  | bit 7: m 7th | bit 8: d 7th
  display_byte[29] = 0b11001110;  // bit 1: f 7th  | bit 2: e 7th  | bit 3: g 7th  | bit 4: k 7th  | bit 5: b 1drone battery | bit 6: drone battery percentage | bit 7: c 1drone battery | bit 8: xxx
  display_byte[30] = 0b11001110;  // bit 1: a 1drone battery | bit 2: d 1drone battery | bit 3 : g 1drone battery | bit 4: xxx | bit 5: f 1drone battery | bit 6: top triangle | bit 7: e 1drone battery | bit 8: xxx
  display_byte[31] = 0b11101100;  // bit 1: b 10 drone battery | bit 2: 3rd drone battery | bit 3: c 10 drone battery | bit 4: xxx | bit 5: a 10 drone battery | bit 6: d 10 drone battery | bit 7: g 10 drone battery | bit 8: xxx
  display_byte[32] = 0b11101111;  // bit 1: f 10 drone battery | bit 2: 100 drone battery | bit 3: e 10 drone battery | bit 4: xxx | bit 5: drone battery box | bit 6: 2nd drone battery | bit 7: 1st drone battery | bit 8: xxx
  display_byte[33] = 0b11000001;  // bit 1: b 6th  | bit 2: c 6th  | bit 3: h 6th  | bit 4: i 6th  | bit 5: n 6th  | bit 6: j 6th  | bit 7: m 6th  | bit 8: d 6th
  display_byte[34] = 0b10001111;  // bit 1: a 6th  | bit 2: g 6th  | bit 3: l 6th  | bit 4: e 6th  | bit 5: b 5th  | bit 6: c 5th  | bit 7: f 6th  | bit 8: k 6th
  display_byte[35] = 0b10000001;  // bit 1: a 5th  | bit 2: h 5th  | bit 3: n 5th  | bit 4: i 5th  | bit 5: l 5th  | bit 6: j 5th  | bit 7: m 5th  | bit 8: d 5th
  display_byte[36] = 0b11001100;  // bit 1: f 5th  | bit 2: e 5th  | bit 3: g 5th  | bit 4: k 5th  | bit 5: b 4th  | bit 6: c 4th  | bit 7: h 4th  | bit 8: i 4th
  display_byte[37] = 0b00011000;  // bit 1: n 4th  | bit 2: j 4th  | bit 3: m 4th  | bit 4: d 4th  | bit 5: a 4th  | bit 6: g 4th  | bit 7: l 4th  | bit 8: k 4th
  display_byte[38] = 0b11111000;  // bit 1: b 3rd  | bit 2: c 3rd  | bit 3: f 4th  | bit 4: e 4th  | bit 5: a 3rd  | bit 6: h 3rd  | bit 7: n 3rd  | bit 8: i 3rd
  display_byte[39] = 0b00011100;  // bit 1: l 3rd  | bit 2: j 3rd  | bit 3: m 3rd  | bit 4: d 3rd  | bit 5: f 3rd  | bit 6: e 3rd  | bit 7: g 3rd  | bit 8: k 3rd
  display_byte[40] = 0b01100110;  // bit 1: t 5th named rssi | bit 2: b 2nd named rssi | bit 3: b 5th named rssi | bit 4: xxx  | bit 5: t 4th named rssi | bit 6: b 3rd named rssi | bit 7: b 4th named rssi | bit 8: xxx
  display_byte[41] = 0b00001111;  // bit 1: t 3rd named rssi | bit 2: t 1st named rssi | bit 3: t 2nd named rssi | bit 4: xxx  | bit 5: b 4th rssi name  | bit 6: b 1st named rssi | bit 7: c 4th rssi name  | bit 8: xxx
  display_byte[42] = 0b00001001;  // bit 1: n 4th rssi name  | bit 2: i 4th rssi name  | bit 3: h 4th rssi name  | bit 4: xxx  | bit 5: a 4th rssi name  | bit 6: j 4th rssi name  | bit 7: m 4th rssi name  | bit 8: a 4th rssi name
  display_byte[43] = 0b00001110;  // bit 1: l 4th rssi name  | bit 2: k 4th rssi name  | bit 3: g 4th rssi name  | bit 4: xxx  | bit 5: f 4th rssi name  | bit 6: d 4th rssi name  | bit 7: e 4th rssi name  | bit 8: xxx
  display_byte[44] = 0b10100000;  // bit 1: b 3rd rssi name  | bit 2: xxx              | bit 3: c 3rd rssi name  | bit 4: xxx  | bit 5: n 3rd rssi name  | bit 6: i 3rd rssi name  | bit 7: h 3rd rssi name  | bit 8: xxx
  display_byte[45] = 0b10000000;  // bit 1: a 3rd rssi name  | bit 2: j 3rd rssi name  | bit 3: m 3rd rssi name  | bit 4: xxx  | bit 5: l 3rd rssi name  | bit 6: k 3rd rssi name  | bit 7: g 3rd rssi name  | bit 8: xxx
  display_byte[46] = 0b11101010;  // bit 1: f 3rd rssi name  | bit 2: d 3rd rssi name  | bit 3: e 3rd rssi name  | bit 4: xxx  | bit 5: b 2nd rssi name  | bit 6: xxx              | bit 7: c 2nd rssi name  | bit 8: xxx
  display_byte[47] = 0b00001000;  // bit 1: n 2nd rssi name  | bit 2: i 2nd rssi name  | bit 3: h 2nd rssi name  | bit 4: xxx  | bit 5: a 2nd rssi name  | bit 6: j 2nd rssi name  | bit 7: m 2nd rssi name  | bit 8: xxx
  display_byte[48] = 0b00001110;  // bit 1: l 2nd rssi name  | bit 2: k 2nd rssi name  | bit 3: g 2nd rssi name  | bit 4: xxx  | bit 5: f 2nd rssi name  | bit 6: d 2nd rssi name  | bit 7: e 2nd rssi name  | bit 8: xxx
  display_byte[49] = 0b10100000;  // bit 1: b 1st rssi name  | bit 2: xxx              | bit 3: c 1st rssi name  | bit 4: xxx  | bit 5: n 1st rssi name  | bit 6: i 1st rssi name  | bi t7: h 1st rssi name  | bit 8: xxx
  display_byte[50] = 0b10000000;  // bit 1: a 1st rssi name  | bit 2: j 3rd rssi name  | bit 3: m 3rd rssi name  | bit 4: xxx  | bit 5: l 3rd rssi name  | bit 6: k 3rd rssi name  | bit 7: g 3rd rssi name  | bit 8: xxx
  display_byte[51] = 0b11101100;  // bit 1: f 1st rssi name  | bit 2: d 1st rssi name  | bit 3: e 1st rssi name  | bit 4: xxx  | bit 5: b 2nd | bit 6: c 2nd | bit 7: h 2nd | bit 8: i 2nd 
  display_byte[52] = 0b00011000;  // bit 1: n 2nd | bit 2: j 2nd | bit 3: m 2nd | bit 4: d 2nd | bit 5: a 2nd | bit 6: g 2nd | bit 7: l 2nd | bit 8: k 2nd 
  display_byte[53] = 0b11111000;  // bit 1: b 1st | bit 2: c 1st | bit 3: f 2nd | bit 4: e 2nd | bit 5: a 1st | bit 6: h 1st | bit 7: n 1st | bit 8: i 1st
  display_byte[54] = 0b00011100;  // bit 1: l 1st | bit 2: j 1st | bit 3: m 1st | bit 4: d 1st | bit 5: f 1st | bit 6: e 1st | bit 7: g 1st | bit 8: k 1st
  display_byte[55] = 0b00011010;  // bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: left graph box | bit 5: b 1speed | bit 6: xxx | bit 7: c 1speed | bit 8: xxx
  display_byte[56] = 0b11001010;  // bit 1: a 1speed | bit 2: d 1speed | bit 3: g 1speed | bit 4: 6th left graph | bit 5: f 1speed | bit 6: xxx | bit 7: e 1speed | bit 8: xxx
  display_byte[57] = 0b10101100;  // bit 1: b 10 speed | bit 2: right dot speed | bit 3: c 10 speed | bit 4: 5th left graph | bit 5: a 10 speed | bit 6: d 10 speed | bit 7: g 10 speed | bit 8: 4th left graph
  display_byte[58] = 0b11101010;  // bit 1: f 10 speed | bit 2: kmh | bit 3: e 10 speed | bit 4: 3rd left graph | bit 5: b 100 speed | bit 6: left dot speed | bit 7: c 100 speed | bit 8: xxx
  display_byte[59] = 0b11001010;  // bit 1: a 100 speed | bit 2: d 100 speed | bit 3: g 100 speed | bit 4: 2nd left graph | bit 5: f 100 speed | bit 6: mph | bit 7: e 100 speed | bit 8: 1st left graph
  display_byte[60] = 0b11010011;  // bit 1: sd | bit 2: c tenth ev | bit 3: ev | bit 4: b tenth ev | bit 5: sport | bit 6: g tenth ev | bit 7: d tenth ev | bit 8: a tenth ev
  display_byte[61] = 0b01010111;  // bit 1: xxx | bit 2: e tenth ev | bit 3: xxx | bit 4: f tenth ev | bit 5: xxx | bit 6: c 1ev | bit 7: ev dot | bit 8: b 1ev 
  display_byte[62] = 0b00110101;  // bit 1: xxx | bit 2: g 1ev | bit 3: d 1ev | bit 4: a 1ev | bit 5: xxx | bit 6: e 1ev | bit 7: xxx | bit 8: f 1ev
  display_byte[63] = 0b01110000;  // bit 1: rec | bit 2: c 10 ev | bit 3: d 10 ev | bit 4: b 10 ev | bit 5: xxx | bit 6: g 10 ev | bit 7: j 10 ev | bit 8: m 10 ev
  display_byte[64] = 0b01110111;  // bit 1: xxx | bit 2: f 10 ev | bit 3: e 10 ev | bit 4: a 10 ev | bit 5: vision | bit 6: c 1tx battery | bit 7: tx battery percentage | bit 8: b 1tx battery
  display_byte[65] = 0b10111111;  // bit 1: 1st tx battery | bit 2: g 1tx battery | bit 3: d 1tx battery | bit 4: a 1tx battery | bit 5: 2nd tx battery | bit 6: e 1tx battery | bit 7: tx battery box | bit 8: f 1tx battery
  display_byte[66] = 0b11110011;  // bit 1: 3rd tx battery | bit 2: c 10 tx battery | bit 3: tx battery | bit 4: b 10 tx battery | bit 5: xxx | bit 6: g 10 tx battery | bit 7: d 10 tx battery | bit 8: a 10 tx battery 
  display_byte[67] = 0b01110001;  // bit 1: xxx | bit 2: e 10 tx battery | bit 3: 100 tx battery | bit 4: f 10 tx battery | bit 5: xxx | bit 6: xxx | bit 7: xxx | bit 8: xxx
  display_byte[68] = 0000000000;  // bit 1:
}
