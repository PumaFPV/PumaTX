void Page1(){
  
  //Throttle
  u8g2.setCursor(0, 8); 
  u8g2.print("Throttle:");
  u8g2.setCursor(0, 16);
  u8g2.print(Throttle.Reading);
  u8g2.setCursor(0, 24);
  u8g2.print(Throttle.Output);
  
  //Yaw
  u8g2.setCursor(0, 32); 
  u8g2.print("Yaw:");
  u8g2.setCursor(0, 40);
  u8g2.print(Yaw.Reading);
  u8g2.setCursor(0, 48);
  u8g2.print(Yaw.Output);
  
  //Pitch
  u8g2.setCursor(64, 8); 
  u8g2.print("Pitch:");
  u8g2.setCursor(64, 16);
  u8g2.print(Pitch.Reading);
  u8g2.setCursor(64, 24);
  u8g2.print(Pitch.Output);
  
  //Roll
  u8g2.setCursor(64, 32); 
  u8g2.print("Roll:");
  u8g2.setCursor(64, 40);
  u8g2.print(Roll.Reading);
  u8g2.setCursor(64, 48);
  u8g2.print(Roll.Output);

  u8g2.setCursor(0, 56); 
  u8g2.print(LeftPot.Output);
  u8g2.setCursor(0, 64);
  //u8g2.print(Arm.Output);
  u8g2.print(Arm.State);

  u8g2.setCursor(64, 56); 
  u8g2.print(RightPot.Output);
  u8g2.setCursor(64, 64);
  u8g2.print(Pre.Output);  
	
}