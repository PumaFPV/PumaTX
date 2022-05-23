void Page2(){

  //Draw Left Joystick
  LeftJoy.x = map(Yaw.Output, -100, 100, 8, 56);   
  LeftJoy.y = map(Throttle.Output, 100, -100, 8, 56);
  u8g2.drawCircle(32, 32, 20);
  u8g2.drawCircle(LeftJoy.x, LeftJoy.y, 2);
  u8g2.drawLine(32, 32, LeftJoy.x, LeftJoy.y);
  
  //Draw Right Joystick
  RightJoy.x = map(Roll.Output, -100, 100, 72, 120);  
  RightJoy.y = map(Pitch.Output, 100, -100, 8, 56);
  u8g2.drawCircle(96, 32, 20);
  u8g2.drawCircle(RightJoy.x, RightJoy.y, 2);
  u8g2.drawLine(96, 32, RightJoy.x, RightJoy.y);

  //Serial.print(xgraph1);
  //Serial.print(" ");
  //Serial.print(ygraph1);
  //Serial.print(" ");
  //Serial.print(xgraph2);
  //Serial.print(" ");
  //Serial.println(ygraph2);
   

}