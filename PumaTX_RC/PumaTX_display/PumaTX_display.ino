#include <Wire.h>

#define I2C_SDA 23
#define I2C_SCL 18

#define BL 22

byte display_byte[68];



void setup_display();



void setup()
{
  Serial.begin(115200);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BL, 0);
  ledcWrite(0, 250);

  Wire.begin(I2C_SDA, I2C_SCL);
  delay(1000);

  setup_display();

  delay(6);

  for(int i = 0; i < 68; i++)
  {
    display_byte[i] = 0x00; 
  }

  display_byte[0] =  0b00000010;  //Choose to display fr or m in B r corner 0bxxxxfxmx f for foot m for meter 
  display_byte[1] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B r 7 s 1st from r
  display_byte[2] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B r 7 s 2nd from r
  display_byte[3] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B r 7 s 3rd from r
  display_byte[4] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | | B r 7 s 4th from r
  display_byte[5] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B r 7 s 5th from r
  display_byte[6] =  0b00000010;  // 0b1234f6m8 | 1st bit is t line | 2nd bit is 2nd arrow up | 3rd bit is 3rd arrow up | 4th bit is 1st arrow down or 2nd arrow down| 6th bit is 1st arrow up | 8th bit 
  display_byte[7] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B m 7 s 1st from r
  display_byte[8] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B m 7 s 2nd from r
  display_byte[9] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B m 7 s 3rd from r
  display_byte[10] = 0b11101111;  // 0bxxx4xxxx | bit 4 is useless | B m 7 s 4th from r
  display_byte[11] = 0b01101100;  // bit 1: ft on B l | bit 2: clearance | bit 3: meter on B l | bit 4: saw teeth on B l | bit 5: t r s for r 7 s on B l | bit 6: B r s for r 7 s on B l | bit 7: m s for r 7 s on B l|  bit 8 is triangle on B l
  display_byte[12] = 0b11111100;  // bit 1: t s for r 7 s on B l | bit 2: B l s for r 7 s on B l| bit 3: t l s for r 7 s on B l| bit 4: B s for r 7 s on B l| bit 5: t r s for l 7 s on B l | bit 6: B r s for l 7 s on B l | bit 7: m s for l 7 s on B l | bit 8: dot on B l
  display_byte[13] = 0b11111010;  // bit 1: t s l 7 s on B l | bit 2: B l s s for l 7 s on B l | bit 3: t l s r 7 s on B l | bit 4: B s for r 7 s on B l | bit 5: t r for unit rpm | bit 6: xxx | bit 7:  B r for unit rpm | bit 8: xxx
  display_byte[14] = 0b11000010;  // bit 1: t s unit rpm | bit 2: B s for unit rpm | bit 3: m s for unit rpm | bit 4: 1st bar r graph | bit 5: xxx | bit 6: xxx | bit 7: B l s for unit rpm | bit 8: xxx |
  display_byte[15] = 0b11101101;  // bit 1: t r s tens rpm | bit 2: RPM | bit 3: B r s tens rpm | bit 4: 2nd bar r graph | bit 5: t s tens rpm | bit 6: B s tens rpm | bit 7: m s tens rpm | bit 8: xxx
  display_byte[16] = 0b10101110;  // bit 1: t l s tens rpm | bit 2: 10 rpm | bit 3: B l s tens rpm | bit 4: 3rd bar r graph | bit 5: t r s hundreds rpm | bit 6: *rpm | bit 7: B r s hundreds rpm | bit 8: 4th bar r graph |
  display_byte[17] = 0b11001110;  // bit 1: t s hundreds rpm | bit 2: B s hundreds rpm | bit 3: m s hundreds rpm | bit 4: 5th bar r graph | bit 5: t l s hundreds rpm | bit 6: thousand rpm | bit 7: B l s hundreds rpm | bit 8: 6th bar r graph
  display_byte[18] = 0b00001100;  // bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: boxes r graph | bit 5: t r 11th  | bit 6: B r 11th  | bit 7: xxx | bit 8: xxx
  display_byte[19] = 0b10000001;  // bit 1: t 11th  | bit 2: m r 11th  | bit 3: t r d 11th  | bit 4: B r d 11th  | bit 5: t l d 11th  | bit 6: B m 11th  | bit 7: t m 11th  | bit 8: B 11th 
  display_byte[20] = 0b11000010;  // bit 1: t l 11th  | bit 2: B l 11th  | bit 3: m l 11th | bit 4: B l d 11th  | bit 5: t 5th radio rssi | bit 6: xxx | bit 7: B 5th radio rssi | bit 8: xxx
  display_byte[21] = 0b01100100;  // bit 1: t 4th radio rssi | bit 2: B 3rd radio rssi | bit 3: B 4th radio rassi | bit 4: xxx | bit 5: t 3rd radio rssi | bit 6: B 2nd radio rssi | bit 7: t 2nd radio rssi | bit 8: xxx
  display_byte[22] = 0b11001100;  // bit 1: radio | bit 2: B 1st radio rssi | bit 3: t 1st radio rssi | bit 4: xxx | bit 5: t r 10th  | bit 6: B r 10th  | bit 7: m r 10th  | bit 8: B r d 10th 
  display_byte[23] = 0b00011000;  // bit 1: t r d 10th  | bit 2: B m 10th  | bit 3: t m 10th  | bit 4: B 10th  | bit 5: t 10th  | bit 6: m l 10th  | bit 7: t l d 10th  | bit 8: B l d 10th 
  display_byte[24] = 0b11111000;  // bit 1: t r 9th  | bit 2: B r 9th  | bit 3: t l 9th | bit 4: B l 9th | bit 5: t 9th  | bit 6: m r 9th  | bit 7: t r d 9th  | bit 8: B r d 9th 
  display_byte[25] = 0b00011100;  // bit 1: t l d 9th  | bit 2: B m 9th  | bit 3: t m 9th | bit 4: B 9th | bit 5: t l 9th  | bit 6: B l 9th  | bit 7: l m 9th  | bit 8: B l d 9th 
  display_byte[26] = 0b11000001;  // bit 1: t r 8th  | bit 2: B r 8th  | bit 3: m r 8th | bit 4: B r 8th | bit 5: t r d 8th  | bit 6: B m 8th  | bit 7: t m 8th  | bit 8: B 8th 
  display_byte[27] = 0b10001110;  // bit 1: t 8th | bit 2: l m 8th | bit 3: t l d 8th | bit 4: B l d 8th | bit 5: t r 7th  | bit 6: B r 7th  | bit 7: t l 8th | bit 8: B l 8th
  display_byte[28] = 0b10000000;  // bit 1: t 7th | bit 2: r m 7th | bit 3: t r d 7th | bit 4: b r d 7th | bit 5: t r d 7th | bit 6: b m 7th | bit 7: t m 7th | bit 8: B 7th
  display_byte[29] = 0b11001110;  // bit 1: t l 7th | bit 2: b l 7th | bit 3: m l | bit 4: b l d 7th | bit 5: t r unit drone battery | bit 6: drone battery percentage | bit 7: b r unit drone battery | bit 8: xxx
  display_byte[30] = 0b11001110;  // bit 1: t unit drone battery | bit 2: b unit drone battery | bit 3 : m unit drone battery | bit 4: xxx | bit 5: t l unit drone battery | bit 6: t triangle | bit 7: b l unit drone battery | bit 8: xxx
  display_byte[31] = 0b11101100;  // bit 1: t r tens drone battery | bit 2: 3rd drone battery | bit 3: b l tens drone battery | bit 4: xxx | bit 5: t tens drone battery | bit 6: b tens drone battery | bit 7: m tens drone battery | bit 8: xxx
  display_byte[32] = 0b11101111;  // bit 1: t l tens drone battery | bit 2: hundreds drone battery | bit 3: b l tens drone battery | bit 4: xxx | bit 5: drone battery box | bit 6: 2nd drone battery | bit 7: 1st drone battery | bit 8: xxx
  display_byte[33] = 0b11000001;  // bit 1: t r 6th | bit 2: b r 6th | bit 3: m r 6th | bit 4: b r d 6th | bit 5: t r d 6th | bit 6: b m 6th | bit 7: t m 6th | bit 8: b 6th
  display_byte[34] = 0b10001111;  // bit 1: t 6th | bit 2: m l 6th | bit 3: t l d 6th | bit 4: b l 6th | bit 5: t r 5th | bit 6: b r 5th | bit 7: t l 6th |
  
  }



void loop()
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
  
  Serial.println("begin");
  
  for(int i = 0; i < 68; i++)
  {
    Wire.write(display_byte[i]);
  }
  
  Wire.endTransmission();

  Serial.println("end");
  delay(10);
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
  Wire.write(0xB9);
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
