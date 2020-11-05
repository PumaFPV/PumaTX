void setup_left_mlx()
{
    Wire.beginTransmission(0x0C);
    Wire.write(0x60);// Select Write register command
    Wire.write(0x00);// Set AH = 0x00, BIST disabled
    Wire.write(0x5C);// Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);  // Select 0x0Cess register, (0x00 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);// Request 1 byte of reading
   
    if(Wire.available() == 1)
    { // Read status byte
        unsigned int c = Wire.read();
    }
    
    Wire.beginTransmission(0x0C);// Start I2C Transmission
    Wire.write(0x60);// Select Write register command
    Wire.write(0x02);// Set AH = 0x02
    Wire.write(0xB4);// Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);// Select 0x0Cess register, (0x02 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);// Request 1 byte of reading
    
    if(Wire.available() == 1)
    {  // Read status byte
        unsigned int c = Wire.read();
    }
}

void setup_right_mlx()
{
    Wire.beginTransmission(0x0D);
    Wire.write(0x60);// Select Write register command
    Wire.write(0x00);// Set AH = 0x00, BIST disabled
    Wire.write(0x5C);// Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);  // Select 0x0Cess register, (0x00 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);// Request 1 byte of reading
   
    if(Wire.available() == 1)
    { // Read status byte
        unsigned int c = Wire.read();
    }
    
    Wire.beginTransmission(0x0D);// Start I2C Transmission
    Wire.write(0x60);// Select Write register command
    Wire.write(0x02);// Set AH = 0x02
    Wire.write(0xB4);// Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);// Select 0x0Cess register, (0x02 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);// Request 1 byte of reading
    
    if(Wire.available() == 1)
    {  // Read status byte
        unsigned int c = Wire.read();
    }
}

void loop_left_mlx()
{
    unsigned int reading[7];
 
    Wire.beginTransmission(0x0C);// Start I2C Transmission
    Wire.write(0x3E);// Start single meaurement mode, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
 
    Wire.requestFrom(0x0C, 1);// Request 1 byte of reading
 
    if(Wire.available() == 1)
    {// Read status byte
        unsigned int c = Wire.read();
    }
    //delay(10);
 

    Wire.beginTransmission(0X0C);
    Wire.write(0x4E);// Send read measurement command, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0C, 7);// Request 7 bytes of reading
 
    // Read 7 bytes of reading
    // status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
    if(Wire.available() == 7)
    {
        reading[0] = Wire.read();
        reading[1] = Wire.read();
        reading[2] = Wire.read();
        reading[3] = Wire.read();
        reading[4] = Wire.read();
        reading[5] = Wire.read();
        reading[6] = Wire.read();
        reading[7] = Wire.read();
    }
 

    int xMag = reading[1] * 256 + reading[2];// Convert the reading
    int yMag = reading[3] * 256 + reading[4];

    if (xMag > 50000)
    {
        throttle.reading = xMag - 65535;
    }
    else throttle.reading = xMag;


    if (yMag > 50000)
    {
        yaw.reading = yMag - 65535;
    }
    else yaw.reading = yMag; 

}



void loop_right_mlx()
{
    unsigned int reading[7];
 
    Wire.beginTransmission(0x0D);// Start I2C Transmission
    Wire.write(0x3E);// Start single meaurement mode, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
 
    Wire.requestFrom(0x0D, 1);// Request 1 byte of reading
 
    if(Wire.available() == 1)
    {// Read status byte
        unsigned int c = Wire.read();
    }
    //delay(10);
 

    Wire.beginTransmission(0X0D);
    Wire.write(0x4E);// Send read measurement command, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0D, 7);// Request 7 bytes of reading
 
    // Read 7 bytes of reading
    // status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
    if(Wire.available() == 7)
    {
        reading[0] = Wire.read();
        reading[1] = Wire.read();
        reading[2] = Wire.read();
        reading[3] = Wire.read();
        reading[4] = Wire.read();
        reading[5] = Wire.read();
        reading[6] = Wire.read();
        reading[7] = Wire.read();
    }
 

    int xMag = reading[1] * 256 + reading[2];// Convert the reading
    int yMag = reading[3] * 256 + reading[4];

    if (xMag > 50000)
    {
        pitch.reading = xMag - 65535;
    }
    else pitch.reading = xMag;


    if (yMag > 50000)
    {
        roll.reading = yMag - 65535;
    }
    else roll.reading = yMag; 

}
