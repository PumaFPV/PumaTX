#include <Wire.h>

#define I2C_SDA 23
#define I2C_SCL 18

#define BL 22

byte display_byte[68];



void setup_display();
void display_default();


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

  delay(100);
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
  display_byte[31] = 0b11101100;  // bit 1: b tens drone battery | bit 2: 3rd drone battery | bit 3: e tens drone battery | bit 4: xxx | bit 5: a tens drone battery | bit 6: d tens drone battery | bit 7: g tens drone battery | bit 8: xxx
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
  display_byte[59] = 0b11001010;  // bit 1: 1 hundreds speed | bit 2: d hundreds speed | bit 3: g hundreds speed | bit 4: 2nd left graph | bit 5: f hundreds speed | bit 6: mph | bit 7: e hundreds speed | bit 8: 1st left graph
  display_byte[60] = 0b11010011;  // bit 1: sd | bit 2: c tenth ev | bit 3: ev | bit 4: b tenth ev | bit 5: sport | bit 6: g tenth ev | bit 7: d tenth ev | bit 8: a tenth ev
  display_byte[61] = 0b01010111;  // bit 1: xxx | bit 2: e tenth ev | bit 3: xxx | bit 4: f tenth ev | bit 5: xxx | bit 6: c unit ev | bit 7: ev dot | bit 8: b unit ev 
  display_byte[62] = 0b00110101;  // bit 1: xxx | bit 2: g unit ev | bit 3: d unit ev | bit 4: a unit ev | bit 5: xxx | bit 6: e unit ev | bit 7: xxx | bit 8: f unit ev
  display_byte[63] = 0b01110000;  // bit 1: rec | bit 2: c tens ev | bit 3: d tens ev | bit 4: b tens ev | bit 5: xxx | bit 6: g tens ev | bit 7: j tens ev | bit 8: m tens ev
  display_byte[64] = 0b01110111;  // bit 1: xxx | bit 2: f tens ev | bit 3: e tens ev | bit 4: a tens ev | bit 5: vision | bit 6: c unit tx battery | bit 7: tx battery percentage | bit 8: b unit tx battery
  display_byte[65] = 0b10111111;  // bit 1: 1st tx battery | bit 2: g unit tx battery | bit 3: d  unit tx battery | bit 4: a unit tx battery | bit 5: 2nd tx battery | bit 6: e unit battery | bit 7: tx battery box | bit 8: f unit tx battery
  display_byte[66] = 0b11110011;  // bit 1: 3rd tx battery | bit 2: c tens tx battery | bit 3: tx battery | bit 4: b tens tx battery | bit 5: xxx | bit 6: g tens tx battery | bit 7: d tens tx battery | bit 8: a tens tx battery 
  display_byte[67] = 0b01110001;  // bit 1: xxx | bit 2: e tens tx battery | bit 3: hundreds tx battery | bit 4: f tens tx battery | bit 5: xxx | bit 6: xxx | bit 7: xxx | bit 8: xxx
  display_byte[68] = 0000000000;  // bit 1:
}
