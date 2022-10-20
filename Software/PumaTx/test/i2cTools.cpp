void scannerMlx()
{
  debug.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  mlxI2C.begin();
  for (byte i = 8; i < 120; i++)
  {
    mlxI2C.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (mlxI2C.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s) on mlxI2C");
}

void scannerDisplay()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  displayI2C.begin();
  for (byte i = 8; i < 120; i++)
  {
    displayI2C.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (displayI2C.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s) on displayI2C");
}