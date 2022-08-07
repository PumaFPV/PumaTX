

void Page3(){

  u8g2.setFont(u8g_font_unifont);
  u8g2.setCursor(0, 11);
  u8g2.print("Setup");
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 20);
  u8g2.print(Firmware);
  
  //Voltage
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 50);
  u8g2.print("Voltage:");
  u8g2.setCursor(0, 57);
  u8g2.print(Voltage.State);

}