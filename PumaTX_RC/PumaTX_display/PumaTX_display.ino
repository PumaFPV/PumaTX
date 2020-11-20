#include <Wire.h>

#define I2C_SDA 23
#define I2C_SCL 18



byte display_byte[68];



void setup_display();



void setup()
{
  Serial.begin(115200);
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(1000);

  setup_display();

  delay(6);

  for(int i = 0; i < 68; i++)
  {
    display_byte[i] = 0xFF; 
  }

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
  Wire.write(display_byte[0]);  
  Wire.write(display_byte[1]); 
  Wire.write(display_byte[2]); 
  Wire.write(display_byte[3]); 
  Wire.write(display_byte[4]); 
  Wire.write(display_byte[5]); 
  Wire.write(display_byte[6]); 
  Wire.write(display_byte[7]); 
  Wire.write(display_byte[8]); 
  Wire.write(display_byte[9]); 
  Wire.write(display_byte[10]); 
  Wire.write(display_byte[11]); 
  Wire.write(display_byte[12]); 
  Wire.write(display_byte[13]); 
  Wire.write(display_byte[14]); 
  Wire.write(display_byte[15]); 
  Wire.write(display_byte[16]); 
  Wire.write(display_byte[17]); 
  Wire.write(display_byte[18]); 
  Wire.write(display_byte[19]); 
  Wire.write(display_byte[20]); 
  Wire.write(display_byte[21]); 
  Wire.write(display_byte[22]); 
  Wire.write(display_byte[23]);   
  Wire.write(display_byte[24]); 
  Wire.write(display_byte[25]);
  Wire.write(display_byte[26]);
  Wire.write(display_byte[27]);
  Wire.write(display_byte[28]);
  Wire.write(display_byte[29]);
  Wire.write(display_byte[30]);
  Wire.write(display_byte[31]);
  Wire.write(display_byte[32]);
  Wire.write(display_byte[33]);
  Wire.write(display_byte[34]);
  Wire.write(display_byte[35]);
  Wire.write(display_byte[36]);
  Wire.write(display_byte[37]);
  Wire.write(display_byte[38]);
  Wire.write(display_byte[39]);
  Wire.write(display_byte[40]);
  Wire.write(display_byte[41]);
  Wire.write(display_byte[42]);
  Wire.write(display_byte[43]);
  Wire.write(display_byte[44]);
  Wire.write(display_byte[45]);
  Wire.write(display_byte[46]);
  Wire.write(display_byte[47]);
  Wire.write(display_byte[48]);
  Wire.write(display_byte[49]);
  Wire.write(display_byte[50]);
  Wire.write(display_byte[51]);
  Wire.write(display_byte[52]);
  Wire.write(display_byte[53]);
  Wire.write(display_byte[54]);
  Wire.write(display_byte[55]);
  Wire.write(display_byte[56]);
  Wire.write(display_byte[57]);
  Wire.write(display_byte[58]);
  Wire.write(display_byte[59]);
  Wire.write(display_byte[60]);
  Wire.write(display_byte[61]);
  Wire.write(display_byte[62]);
  Wire.write(display_byte[63]);
  Wire.write(display_byte[64]);
  Wire.write(display_byte[65]);
  Wire.write(display_byte[66]);
  Wire.write(display_byte[67]);


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
