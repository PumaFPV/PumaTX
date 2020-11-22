#include <Wire.h>

#define I2C_SDA 23
#define I2C_SCL 18

#define BL 22

byte display_byte[68];



void setup_display();
void loop_display(int i);
void display_default();
void set_left_graph(uint8_t bar);
void set_right_graph(uint8_t bar);
void set_rc_rssi(uint8_t bar);
void set_named_rssi(uint8_t bar);
void set_tx_battery_bar(uint8_t bar);
void set_tx_battery_percentage(uint8_t percentage);
void set_drone_battery_bar(uint8_t bar);
void set_rpm(int rpm); 
void draw_rpm(uint8_t display, char digit);
byte char_to_7_segment(char digit);
void set_speed(int speed);
void set_distance(int distance);

void setup()
{
  Serial.begin(115200);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BL, 0);
  ledcWrite(0, 255);

  Wire.begin(I2C_SDA, I2C_SCL);
  delay(1000);

  setup_display();

  delay(6);

  for(int i = 0; i < 68; i++)
  {
    display_byte[i] = 0x00; 
  }
  display_default();
 

}



void loop()
{
  for(int i = 0; i < 999; i++)
  {
    loop_display(i);
  }
  for(int i = 6; i > 1; i--)
  {
    loop_display(i);
  }
}

void loop_display(int i)
{

  set_left_graph(i / 120);
  set_right_graph(6 - (i / 120));
  set_rc_rssi(i / 200);
  set_named_rssi(i / 200);
  set_tx_battery_bar(i / 300);
  set_tx_battery_percentage(100);
  set_drone_battery_bar(i / 300);
  set_drone_battery_percentage(100);
  set_rpm(2000);
  set_speed(275);
  set_distance(23546);
  
  Serial.println("begin");
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

  delay(1);
 
}

void set_left_graph(uint8_t bar)
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
}

void set_right_graph(uint8_t bar)
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
}

void set_rc_rssi(uint8_t bar)
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
   
}

void set_named_rssi(uint8_t bar)
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
}

void set_tx_battery_bar(uint8_t bar)
{
  byte tx_battery = 0b000;
  
  tx_battery = pow(2, bar) - 1;
   
  //1st bar
  bitWrite(display_byte[65], 7, bitRead(tx_battery, 0));
  //2nd bar
  bitWrite(display_byte[65], 3 ,bitRead(tx_battery, 1));
  //3rd bar
  bitWrite(display_byte[66], 7, bitRead(tx_battery, 2));
}

void set_tx_battery_percentage(uint8_t percentage)
{
  uint8_t tx_battery_percentage_hundred = percentage / 100;
  uint8_t tx_battery_percentage_tens = (percentage - tx_battery_percentage_hundred * 100 ) / 10;
  uint8_t tx_battery_percentage_units= (percentage - tx_battery_percentage_hundred * 100 - tx_battery_percentage_tens * 10);

  draw_tx_battery_percentage(1, tx_battery_percentage_units);
  draw_tx_battery_percentage(2, tx_battery_percentage_tens);
  bitWrite(display_byte[67], 5, tx_battery_percentage_hundred);
}

void draw_tx_battery_percentage(uint8_t display, char digit)
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // tx battery  unit 
      bitWrite(display_byte[65], 5, bitRead(segment, 0));  //a
      bitWrite(display_byte[64], 0, bitRead(segment, 1));  //b
      bitWrite(display_byte[64], 2, bitRead(segment, 2));  //c
      bitWrite(display_byte[65], 5, bitRead(segment, 3));  //d
      bitWrite(display_byte[65], 2, bitRead(segment, 4));  //e
      bitWrite(display_byte[65], 0, bitRead(segment, 5));  //f
      bitWrite(display_byte[65], 6, bitRead(segment, 6));  //g
      break;
    case 2: // tx battery tens
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

void set_drone_battery_bar(uint8_t bar)
{
  byte drone_battery = 0b000;

  drone_battery = pow(2, bar) - 1;

  //1st bar
  bitWrite(display_byte[31], 6, bitRead(drone_battery, 0));
  //2nd bar
  bitWrite(display_byte[32], 2 ,bitRead(drone_battery, 1));
  //3rd bar
  bitWrite(display_byte[32], 1, bitRead(drone_battery, 2));
}

void set_drone_battery_percentage(uint8_t percentage)
{
  uint8_t drone_battery_percentage_hundred = percentage / 100;
  uint8_t drone_battery_percentage_tens = (percentage - drone_battery_percentage_hundred * 100 ) / 10;
  uint8_t drone_battery_percentage_units= (percentage - drone_battery_percentage_hundred * 100 - drone_battery_percentage_tens * 10);

  draw_drone_battery_percentage(1, drone_battery_percentage_units);
  draw_drone_battery_percentage(2, drone_battery_percentage_tens);
  bitWrite(display_byte[32], 6, drone_battery_percentage_hundred);

}

void draw_drone_battery_percentage(uint8_t display, char digit)
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // drone battery  unit 
      bitWrite(display_byte[30], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[29], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[29], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[30], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[30], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[30], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[30], 5, bitRead(segment, 6));  //g
      break;
    case 2: // drone battery tens
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


void set_rpm(int rpm)
{
  bool rpm10 = 0;
  
  if(rpm > 1999)
  {
    rpm10 = 1;
    rpm = rpm / 10; 
  }

  int rpm_units = 0;
  int rpm_tens = 0;
  int rpm_hundreds = 0;
  int rpm_thousands = 0;

  rpm_thousands = rpm % 10000 / 1000;
  rpm_hundreds  = rpm % 1000 / 100;
  rpm_tens      = rpm % 100 / 10;
  rpm_units     = rpm % 10;

  draw_rpm(1, rpm_units);
  draw_rpm(2, rpm_tens);
  draw_rpm(3, rpm_hundreds);
  bitWrite(display_byte[17], 2, rpm_thousands);
  bitWrite(display_byte[16], 2, rpm10);
  bitWrite(display_byte[16], 6, rpm10);
  
}

void draw_rpm(uint8_t display, char digit)
{

  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: //rpm unit 
      bitWrite(display_byte[14], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[13], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[13], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[14], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[14], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[14], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[14], 5, bitRead(segment, 6));  //g
      break;
    case 2: // rpm tens
      bitWrite(display_byte[15], 3, bitRead(segment, 0));  //a
      bitWrite(display_byte[15], 7, bitRead(segment, 1));  //b
      bitWrite(display_byte[15], 5, bitRead(segment, 2));  //c
      bitWrite(display_byte[15], 2, bitRead(segment, 3));  //d
      bitWrite(display_byte[16], 5, bitRead(segment, 4));  //e
      bitWrite(display_byte[16], 7, bitRead(segment, 5));  //f
      bitWrite(display_byte[15], 1, bitRead(segment, 6));  //g
      break;
    case 3: // rpm hundreds
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

void set_speed(int speed)
{

  int speed_units = 0;
  int speed_tens = 0;
  int speed_hundreds = 0;

  speed_hundreds = speed % 1000;
  speed_tens     = speed % 100;
  speed_units    = speed % 10;
  
  draw_speed(1, speed_units);
  draw_speed(2, speed_tens);
  draw_speed(3, speed_hundreds);
  
}

void draw_speed(uint8_t display, char digit)
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // speed unit 
      bitWrite(display_byte[56], 7, bitRead(segment, 0));  //a
      bitWrite(display_byte[55], 3, bitRead(segment, 1));  //b
      bitWrite(display_byte[55], 1, bitRead(segment, 2));  //c
      bitWrite(display_byte[56], 6, bitRead(segment, 3));  //d
      bitWrite(display_byte[56], 1, bitRead(segment, 4));  //e
      bitWrite(display_byte[56], 3, bitRead(segment, 5));  //f
      bitWrite(display_byte[56], 5, bitRead(segment, 6));  //g
      break;
    case 2: // speed tens
      bitWrite(display_byte[57], 3, bitRead(segment, 0));  //a
      bitWrite(display_byte[57], 7, bitRead(segment, 1));  //b
      bitWrite(display_byte[57], 5, bitRead(segment, 2));  //c
      bitWrite(display_byte[57], 2, bitRead(segment, 3));  //d
      bitWrite(display_byte[58], 5, bitRead(segment, 4));  //e
      bitWrite(display_byte[58], 7, bitRead(segment, 5));  //f
      bitWrite(display_byte[57], 1, bitRead(segment, 6));  //g
      break;
    case 3: // speed hundreds
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

void set_distance(int distance)
{
 
  int distance_1 = 0;
  int distance_10 = 0;
  int distance_100 = 0;
  int distance_1000 = 0;
  int distance_10000 = 0;

  distamce_10000 = speed % 100000 / 10000;
  distance_1000  = speed % 10000 / 1000;
  distance_100   = speed % 1000/ 100;
  distance_10    = ;
  distance_1     = (speed - distance_hundreds * 100 - speed_tens * 10);

  draw_distance(1, distance_units);
  draw_distance(2, distance_tens);
  draw_distance(3, distance_hundreds);
  
}

void draw_distance(uint8_t display, char digit)
{
  byte segment = char_to_7_segment(digit);
  
  switch(display)
  {
    case 1: // distance unit 
      bitWrite(display_byte[], , bitRead(segment, 0));  //a
      bitWrite(display_byte[], , bitRead(segment, 1));  //b
      bitWrite(display_byte[], , bitRead(segment, 2));  //c
      bitWrite(display_byte[], , bitRead(segment, 3));  //d
      bitWrite(display_byte[], , bitRead(segment, 4));  //e
      bitWrite(display_byte[], , bitRead(segment, 5));  //f
      bitWrite(display_byte[], , bitRead(segment, 6));  //g
      break;
    case 2: // distance tens
      bitWrite(display_byte[], , bitRead(segment, 0));  //a
      bitWrite(display_byte[], , bitRead(segment, 1));  //b
      bitWrite(display_byte[], , bitRead(segment, 2));  //c
      bitWrite(display_byte[], , bitRead(segment, 3));  //d
      bitWrite(display_byte[], , bitRead(segment, 4));  //e
      bitWrite(display_byte[], , bitRead(segment, 5));  //f
      bitWrite(display_byte[], , bitRead(segment, 6));  //g
      break;
    case 3: // distance hundreds
      bitWrite(display_byte[], , bitRead(segment, 0));  //a
      bitWrite(display_byte[], , bitRead(segment, 1));  //b
      bitWrite(display_byte[], , bitRead(segment, 2));  //c
      bitWrite(display_byte[], , bitRead(segment, 3));  //d
      bitWrite(display_byte[], , bitRead(segment, 4));  //e
      bitWrite(display_byte[], , bitRead(segment, 5));  //f
      bitWrite(display_byte[], , bitRead(segment, 6));  //g
      break;
  }
}

byte char_to_7_segment(char digit)
{
  byte 7_digit[] = {63,6,91,79,102,109,125,7,127,111};
 
  return 7_digit[digit]; 
}

void display_default()
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
  display_byte[11] = 0b01111101;  // bit 1: ft on bottom left | bit 2: clearance | bit 3: meter on bottom left | bit 4: saw teeth on bottom left | bit 5: b unit clear | bit 6: c unit clear | bit 7: g unit clear|  bit 8 is triangle on bottom left
  display_byte[12] = 0b11111101;  // bit 1: a unit clear | bit 2: e unit clear | bit 3: f unit clear | bit 4: d unit clear | bit 5: b tens clear | bit 6: c tens clear | bit 7: g tens clear | bit 8: dot clear
  display_byte[13] = 0b11111010;  // bit 1: a tens clear | bit 2: e tens clear | bit 3: f tens clear | bit 4: d tens clear | bit 5: b unit rpm | bit 6: xxx | bit 7: c unit rpm | bit 8: xxx
  display_byte[14] = 0b11001010;  // bit 1: a unit rpm | bit 2: d unit rpm | bit 3: g unit rpm | bit 4: 1st bar r graph | bit 5: f unit rpm | bit 6: xxx | bit 7: e unit rpm | bit 8: xxx 
  display_byte[15] = 0b11101101;  // bit 1: b tens rpm | bit 2: RPM | bit 3: c tens rpm | bit 4: 2nd bar r graph | bit 5: a tens rpm | bit 6: d tens rpm | bit 7: g tens rpm | bit 8: xxx
  display_byte[16] = 0b10101010;  // bit 1: f tens rpm | bit 2: 10 rpm | bit 3: e tens rpm | bit 4: 3rd bar r graph | bit 5: b hundreds rpm | bit 6: *rpm | bit 7: c hundreds rpm | bit 8: 4th bar r graph 
  display_byte[17] = 0b11001110;  // bit 1: a hundreds rpm | bit 2: d hundreds rpm | bit 3: g hundreds rpm | bit 4: 5th bar r graph | bit 5: f hundreds rpm | bit 6: thousand rpm | bit 7: e hundreds rpm | bit 8: 6th bar r graph
  display_byte[18] = 0b00011100;  // bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: right graph box | bit 5: b 11th  | bit 6: c 11th  | bit 7: xxx | bit 8: xxx
  display_byte[19] = 0b10000001;  // bit 1: a 11th | bit 2: h 11th | bit 3: n 11th | bit 4: i 11th | bit 5: l 11th | bit 6: j 11th | bit 7: m 11th | bit 8: d 11th 
  display_byte[20] = 0b11000010;  // bit 1: f 11th | bit 2: e 11th | bit 3: g 11th | bit 4: k 11th | bit 5: t 5th radio rssi | bit 6: xxx | bit 7: b 5th radio rssi | bit 8: xxx
  display_byte[21] = 0b01100100;  // bit 1: t 4th radio rssi | bit 2: b 3rd radio rssi | bit 3: b 4th radio rassi | bit 4: xxx | bit 5: t 3rd radio rssi | bit 6: b 2nd radio rssi | bit 7: t 2nd radio rssi | bit 8: xxx
  display_byte[22] = 0b11001100;  // bit 1: radio | bit 2: b 1st radio rssi | bit 3: t 1st radio rssi | bit 4: xxx | bit 5: b 10th | bit 6: c 10th | bit 7: h 10th | bit 8: i 10th 
  display_byte[23] = 0b00011000;  // bit 1: n 10th | bit 2: j 10th | bit 3: m 10th | bit 4: d 10th | bit 5: a 10th | bit 6: g 10th | bit 7: l 10th | bit 8: k 10th 
  display_byte[24] = 0b11111000;  // bit 1: b 9th  | bit 2: c 9th  | bit 3: f 9th  | bit 4: e 9th  | bit 5: a 9th  | bit 6: h 9th  | bit 7: n 9th  | bit 8: i 9th 
  display_byte[25] = 0b00011100;  // bit 1: l 9th  | bit 2: j 9th  | bit 3: m 9th  | bit 4: d 9th  | bit 5: f 9th  | bit 6: e 9th  | bit 7: g 9th  | bit 8: k 9th 
  display_byte[26] = 0b11000001;  // bit 1: b 8th  | bit 2: c 8th  | bit 3: h 8th  | bit 4: c 8th  | bit 5: n 8th  | bit 6: j 8th  | bit 7: m 8th  | bit 8: d 8th 
  display_byte[27] = 0b10001111;  // bit 1: a 8th  | bit 2: g 8th  | bit 3: l 8th  | bit 4: k 8th  | bit 5: b 7th  | bit 6: c 7th  | bit 7: f 8th | bit 8: e 8th
  display_byte[28] = 0b10000001;  // bit 1: a 7th  | bit 2: h 7th  | bit 3: n 7th  | bit 4: i 7th  | bit 5: l 7th  | bit 6: j 7th  | bit 7: m 7th | bit 8: d 7th
  display_byte[29] = 0b11001110;  // bit 1: f 7th  | bit 2: e 7th  | bit 3: g 7th  | bit 4: k 7th  | bit 5: b unit drone battery | bit 6: drone battery percentage | bit 7: c unit drone battery | bit 8: xxx
  display_byte[30] = 0b11001110;  // bit 1: a unit drone battery | bit 2: d unit drone battery | bit 3 : g unit drone battery | bit 4: xxx | bit 5: f unit drone battery | bit 6: top triangle | bit 7: e unit drone battery | bit 8: xxx
  display_byte[31] = 0b11101100;  // bit 1: b tens drone battery | bit 2: 3rd drone battery | bit 3: c tens drone battery | bit 4: xxx | bit 5: a tens drone battery | bit 6: d tens drone battery | bit 7: g tens drone battery | bit 8: xxx
  display_byte[32] = 0b11101111;  // bit 1: f tens drone battery | bit 2: hundreds drone battery | bit 3: e tens drone battery | bit 4: xxx | bit 5: drone battery box | bit 6: 2nd drone battery | bit 7: 1st drone battery | bit 8: xxx
  display_byte[33] = 0b11000001;  // bit 1: b 6th  | bit 2: c 6th  | bit 3: h 6th  | bit 4: i 6th  | bit 5: n 6th  | bit 6: j 6th  | bit 7: m 6th  | bit 8: d 6th
  display_byte[34] = 0b10001111;  // bit 1: a 6th  | bit 2: g 6th  | bit 3: l 6th  | bit 4: e 6th  | bit 5: b 5th  | bit 6: c 5th  | bit 7: f 6th  | bit 8: e 6th
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
  display_byte[49] = 0b10100000;  // bit 1: b 1st rssi name  | bit 2: xxx              | bit 3: c 1st rssi name  | bit 4: xxx  | bit 5: n 1st rssi name  | bit 6: i 1st rssi name  | bit 7: h 1st rssi name  | bit 8: xxx
  display_byte[50] = 0b10000000;  // bit 1: a 1st rssi name  | bit 2: j 3rd rssi name  | bit 3: m 3rd rssi name  | bit 4: xxx  | bit 5: l 3rd rssi name  | bit 6: k 3rd rssi name  | bit 7: g 3rd rssi name  | bit 8: xxx
  display_byte[51] = 0b11101100;  // bit 1: f 1st rssi name  | bit 2: d 1st rssi name  | bit 3: e 1st rssi name  | bit 4: xxx  | bit 5: b 2nd  | bit 6: c 2nd | bit 7: h 2nd | bit 8: i 2nd 
  display_byte[52] = 0b00011000;  // bit 1: n 2nd | bit 2: j 2nd | bit 3: m 2nd | bit 4: d 2nd | bit 5: a 2nd | bit 6: g 2nd | bit 7: l 2nd | bit 8: k 2nd 
  display_byte[53] = 0b11111000;  // bit 1: b 1st | bit 2: c 1st | bit 3: f 2nd | bit 4: e 2nd | bit 5: a 1st | bit 6: h 1st | bit 7: n 1st | bit 8: i 2nd
  display_byte[54] = 0b00011100;  // bit 1: l 1st | bit 2: j 1st | bit 3: m 1st | bit 4: d 1st | bit 5: f 1st | bit 6: e 1st | bit 7: g 1st | bit 8: k 1st
  display_byte[55] = 0b00011010;  // bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: left graph box | bit 5: b unit speed | bit 6: xxx | bit 7: c unit speed | bit 8: xxx
  display_byte[56] = 0b11001010;  // bit 1: a unit speed | bit 2: d unit speed | bit 3: g unit speed | bit 4: 6th left graph | bit 5: f unit speed | bit 6: xxx | bit 7: e unit speed | bit 8: xxx
  display_byte[57] = 0b10101100;  // bit 1: b tens speed | bit 2: right dot speed | bit 3: c tens speed | bit 4: 5th left graph | bit 5: a tens speed | bit 6: d tens speed | bit 7: g tens speed | bit 8: 4th left graph
  display_byte[58] = 0b11101010;  // bit 1: f tens speed | bit 2: kmh | bit 3: e tens speed | bit 4: 3rd left graph | bit 5: b hundreds speed | bit 6: left dot speed | bit 7: c hundreds speed | bit 8: xxx
  display_byte[59] = 0b11001010;  // bit 1: a hundreds speed | bit 2: d hundreds speed | bit 3: g hundreds speed | bit 4: 2nd left graph | bit 5: f hundreds speed | bit 6: mph | bit 7: e hundreds speed | bit 8: 1st left graph
  display_byte[60] = 0b11010011;  // bit 1: sd | bit 2: c tenth ev | bit 3: ev | bit 4: b tenth ev | bit 5: sport | bit 6: g tenth ev | bit 7: d tenth ev | bit 8: a tenth ev
  display_byte[61] = 0b01010111;  // bit 1: xxx | bit 2: e tenth ev | bit 3: xxx | bit 4: f tenth ev | bit 5: xxx | bit 6: c unit ev | bit 7: ev dot | bit 8: b unit ev 
  display_byte[62] = 0b00110101;  // bit 1: xxx | bit 2: g unit ev | bit 3: d unit ev | bit 4: a unit ev | bit 5: xxx | bit 6: e unit ev | bit 7: xxx | bit 8: f unit ev
  display_byte[63] = 0b01110000;  // bit 1: rec | bit 2: c tens ev | bit 3: d tens ev | bit 4: b tens ev | bit 5: xxx | bit 6: g tens ev | bit 7: j tens ev | bit 8: m tens ev
  display_byte[64] = 0b01110111;  // bit 1: xxx | bit 2: f tens ev | bit 3: e tens ev | bit 4: a tens ev | bit 5: vision | bit 6: c unit tx battery | bit 7: tx battery percentage | bit 8: b unit tx battery
  display_byte[65] = 0b10111111;  // bit 1: 1st tx battery | bit 2: g unit tx battery | bit 3: d unit tx battery | bit 4: a unit tx battery | bit 5: 2nd tx battery | bit 6: e unit tx battery | bit 7: tx battery box | bit 8: f unit tx battery
  display_byte[66] = 0b11110011;  // bit 1: 3rd tx battery | bit 2: c tens tx battery | bit 3: tx battery | bit 4: b tens tx battery | bit 5: xxx | bit 6: g tens tx battery | bit 7: d tens tx battery | bit 8: a tens tx battery 
  display_byte[67] = 0b01110001;  // bit 1: xxx | bit 2: e tens tx battery | bit 3: hundreds tx battery | bit 4: f tens tx battery | bit 5: xxx | bit 6: xxx | bit 7: xxx | bit 8: xxx
  display_byte[68] = 0000000000;  // bit 1:
}

void setup_display()
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
  
}
