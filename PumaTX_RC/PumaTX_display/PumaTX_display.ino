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

  display_byte[0] =  0b00000010;  //Choose to display fr or m in bottom right corner 0bxxxxfxmx f for foot m for meter 
  display_byte[1] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom right 7 segment 1st from right
  display_byte[2] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom right 7 segment 2nd from right
  display_byte[3] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom right 7 segment 3rd from right
  display_byte[4] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | | bottom right 7 segment 4th from right
  display_byte[5] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom right 7 segment 5th from right
  display_byte[6] =  0b00000010;  // 0b1234f6m8 | 1st bit is top line | 2nd bit is 2nd arrow up | 3rd bit is 3rd arrow up | 4th bit is 1st arrow down or 2nd arrow down| 6th bit is 1st arrow up | 8th bit 
  display_byte[7] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom mid 7 segment 1st from right
  display_byte[8] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom mid 7 segment 2nd from right
  display_byte[9] =  0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom mid 7 segment 3rd from right
  display_byte[10] = 0b11101111;  // 0bxxx4xxxx | bit 4 is useless | bottom mid 7 segment 4th from right
  display_byte[11] = 0b01101100;  // 0b12345678 | bit 1: ft on bottom left | bit 2: clearance | bit 3: meter on bottom left | bit 4: saw teeth on bottom left | bit 5: top right segment for right 7 segment on bottom left | bit 6: bottom right segment for right 7 segment on bottom left | bit 7: mid segment for right 7 segment on bottom left|  bit 8 is triangle on bottom left
  display_byte[12] = 0b11111100;  // 0b12345678 | bit 1: top segment for right 7 segment on bottom left | bit 2: bottom left segment for right 7 segment on bottom left| bit 3: top left segment for right 7 segment on bottom left| bit 4: bottom segment for right 7 segment on bottom left| bit 5: top right segment for left 7 segment on bottom left | bit 6: bottom right segment for left 7 segment on bottom left | bit 7: mid segment for left 7 segment on bottom left | bit 8: dot on bottom left
  display_byte[13] = 0b11111010;  // 0b12345678 | bit 1: top segment left 7 segment on bottom left | bit 2: bottom left segment segment for left 7 segment on bottom left | bit 3: top left segment right 7 segment on bottom left | bit 4: bottom segment for right 7 segment on bottom left | bit 5: top right for unit rpm | bit 6: xxx | bit 7:  bottom right for unit rpm | bit 8: xxx
  display_byte[14] = 0b11000010;  // 0b12345678 | bit 1: top segment unit rpm | bit 2: bottom segment for unit rpm | bit 3: mid segment for unit rpm | bit 4: 1st bar right graph | bit 5: xxx | bit 6: xxx | bit 7: bottom left segment for unit rpm | bit 8: xxx |
  display_byte[15] = 0b11101101;  // 0b12345678 | bit 1: top right segment tens rpm | bit 2: RPM | bit 3: bottom right segment tens rpm | bit 4: 2nd bar right graph | bit 5: top segment tens rpm | bit 6: bottom segment tens rpm | bit 7: mid segment tens rpm | bit 8: xxx
  display_byte[16] = 0b10101110;  // 0b12345678 | bit 1: top left segment tens rpm | bit 2: 10 rpm | bit 3: bottom left segment tens rpm | bit 4: 3rd bar right graph | bit 5: top right segment hundreds rpm | bit 6: *rpm | bit 7: bottom right segment hundreds rpm | bit 8: 4th bar right graph |
  display_byte[17] = 0b11001110;  // 0b12345678 | bit 1: top segment hundreds rpm | bit 2: bottom segment hundreds rpm | bit 3: mid segment hundreds rpm | bit 4: 5th bar right graph | bit 5: top left segment hundreds rpm | bit 6: thousand rpm | bit 7: bottom left segment hundreds rpm | bit 8: 6th bar right graph
  display_byte[18] = 0b00001100;  // 0b12345678 | bit 1: xxx | bit 2: xxx | bit 3: xxx | bit 4: boxes right graph | bit 5: top right 11th letter | bit 6: bottom right 11th letter | bit 7: xxx | bit 8: xxx
  display_byte[19] = 0b10000001;  // 0b12345678 | bit 1: top 11th letter | bit 2: mid right 11th letter | bit 3: top right diagonal 11th letter | bit 4: bottom right diagonal 11th letter | bit 5: top left diagonal 11th letter | bit 6: bottom mid 11th letter | bit 7: top mid 11th letter | bit 8: bottom 11th letter
  display_byte[20] = 0b11000010;  // 0b12345678 | bit 1: top left 11th letter | bit 2: bottom left 11th letter | bit 3: mid left 11th letter| bit 4: bottom left diagonal 11th letter | bit 5: top 5th radio rssi | bit 6: xxx | bit 7: bottom 5th radio rssi | bit 8: xxx
  display_byte[21] = 0b01100100;  // 0b12345678 | bit 1: top 4th radio rssi | bit 2: bottom 3rd radio rssi | bit 3: bottom 4th radio rassi | bit 4: xxx | bit 5: top 3rd radio rssi | bit 6: bottom 2nd radio rssi | bit 7: top 2nd radio rssi | bit 8: xxx
  display_byte[22] = 0b01001101;  // 0b12345678 | bit 1: radio | bit 2: bottom 1st radio rssi | bit 3: top 1st radio rssi | bit 4: xxx | bit 5: top right 10th letter | bit 6: bottom right 10th letter | bit 7: mid right 10th letter | bit 8:
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
