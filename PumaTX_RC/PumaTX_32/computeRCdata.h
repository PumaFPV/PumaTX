void SetupLeftMLX(){
    Wire.beginTransmission(0x0C);
    Wire.write(0x60);// Select Write register command
    Wire.write(0x00);// Set AH = 0x00, BIST disabled
    Wire.write(0x5C);// Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);  // Select 0x0Cess register, (0x00 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);// Request 1 byte of Reading
   
    if(Wire.available() == 1){ // Read status byte
        unsigned int c = Wire.read();
    }
    
    Wire.beginTransmission(0x0C);// Start I2C Transmission
    Wire.write(0x60);// Select Write register command
    Wire.write(0x02);// Set AH = 0x02
    Wire.write(0xB4);// Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);// Select 0x0Cess register, (0x02 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0C, 1);// Request 1 byte of Reading
    
    if(Wire.available() == 1){  // Read status byte
        unsigned int c = Wire.read();
    }
}

void SetupRightMLX(){
    Wire.beginTransmission(0x0D);
    Wire.write(0x60);// Select Write register command
    Wire.write(0x00);// Set AH = 0x00, BIST disabled
    Wire.write(0x5C);// Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
    Wire.write(0x00);  // Select 0x0Cess register, (0x00 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);// Request 1 byte of Reading
   
    if(Wire.available() == 1){ // Read status byte
        unsigned int c = Wire.read();
    }
    
    Wire.beginTransmission(0x0D);// Start I2C Transmission
    Wire.write(0x60);// Select Write register command
    Wire.write(0x02);// Set AH = 0x02
    Wire.write(0xB4);// Set AL = 0xB4, RES for magnetic measurement = 0
    Wire.write(0x08);// Select 0x0Cess register, (0x02 << 2)
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0D, 1);// Request 1 byte of Reading
    
    if(Wire.available() == 1){  // Read status byte
        unsigned int c = Wire.read();
    }
}

void LoopLeftMLX(){
    unsigned int Reading[7];
 
    Wire.beginTransmission(0x0C);// Start I2C Transmission
    Wire.write(0x3E);// Start single meaurement mode, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
 
    Wire.requestFrom(0x0C, 1);// Request 1 byte of Reading
 
    if(Wire.available() == 1){// Read status byte
        unsigned int c = Wire.read();
    }
    //delay(10);
 

    Wire.beginTransmission(0X0C);
    Wire.write(0x4E);// Send read measurement command, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0C, 7);// Request 7 bytes of Reading
 
    // Read 7 bytes of Reading
    // status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
    if(Wire.available() == 7);{
        Reading[0] = Wire.read();
        Reading[1] = Wire.read();
        Reading[2] = Wire.read();
        Reading[3] = Wire.read();
        Reading[4] = Wire.read();
        Reading[5] = Wire.read();
        Reading[6] = Wire.read();
        Reading[7] = Wire.read();
    }
 

    int xMag = Reading[1] * 256 + Reading[2];// Convert the Reading
    int yMag = Reading[3] * 256 + Reading[4];

    if (xMag > 50000){
        Throttle.Reading = xMag - 65535;
    }
    else Throttle.Reading = xMag;


    if (yMag > 50000){
        Yaw.Reading = yMag - 65535;
    }
    else Yaw.Reading = yMag; 

}

void LoopRightMLX(){
    unsigned int Reading[7];
 
    Wire.beginTransmission(0x0D);// Start I2C Transmission
    Wire.write(0x3E);// Start single meaurement mode, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
 
    Wire.requestFrom(0x0D, 1);// Request 1 byte of Reading
 
    if(Wire.available() == 1){// Read status byte
        unsigned int c = Wire.read();
    }
    //delay(10);
 

    Wire.beginTransmission(0X0D);
    Wire.write(0x4E);// Send read measurement command, ZYX enabled
    Wire.endTransmission();// Stop I2C Transmission
    Wire.requestFrom(0x0D, 7);// Request 7 bytes of Reading
 
    // Read 7 bytes of Reading
    // status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
    if(Wire.available() == 7);{
        Reading[0] = Wire.read();
        Reading[1] = Wire.read();
        Reading[2] = Wire.read();
        Reading[3] = Wire.read();
        Reading[4] = Wire.read();
        Reading[5] = Wire.read();
        Reading[6] = Wire.read();
        Reading[7] = Wire.read();
    }
 

    int xMag = Reading[1] * 256 + Reading[2];// Convert the Reading
    int yMag = Reading[3] * 256 + Reading[4];

    if (xMag > 50000){
        Pitch.Reading = xMag - 65535;
    }
    else Pitch.Reading = xMag;


    if (yMag > 50000){
        Roll.Reading = yMag - 65535;
    }
    else Roll.Reading = yMag; 

}


void ComputeRC4(){
//--------------------------------------------------Throttle

    Throttle.Output = constrain(map(Throttle.Reading, -7600, 8300, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Yaw

    Yaw.Output = constrain(map(Yaw.Reading, -7500, 9200, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Pitch

    Pitch.Output = constrain(map(Pitch.Reading, -7300, 8520, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Roll

    Roll.Output = constrain(map(Roll.Reading, -8300, 8400, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Right Pot
    RightPot.Output = constrain(map(RightPot.State, 440, 3600, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Left Pot

    LeftPot.Process = constrain(map(LeftPot.State, 3570, 440, -100, 100), -100, 100);
    
    if (LeftPot.Process > 75 && millis() - LeftPot.current_time > debouncedelay && LeftPot.intermediate <= 80){
        LeftPot.intermediate += 40;
        LeftPot.current_time = millis();

    }
    //LeftPot.current_time = millis();
    if (LeftPot.Process < -75 && millis() - LeftPot.current_time > debouncedelay && LeftPot.intermediate >= -80){
        LeftPot.intermediate -= 40;
        LeftPot.current_time = millis();

    }

    LeftPot.Output = map(constrain(LeftPot.intermediate, -100, 100), -100, 100, LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Arm
    if (Arm.State == 0 && Arm.Prev == 1 && millis() - Arm.current_time > debouncedelay) {
        if (Arm.Output == LOWER_CHAN){
            Arm.Output = UPPER_CHAN;
        }
        else Arm.Output = LOWER_CHAN;
        Arm.current_time = millis();
    }

    Arm.Prev = Arm.State;

//--------------------------------------------------RTH
    if (RTH.State == 0 && RTH.Prev == 1 && millis() - RTH.current_time > debouncedelay) {
        if (RTH.Output == LOWER_CHAN){
            RTH.Output = UPPER_CHAN;
        }
        else RTH.Output = LOWER_CHAN;
    RTH.current_time = millis();    
    }
    RTH.Prev = RTH.State;

//--------------------------------------------------Pre
    Pre.Output = map(Pre.State, 0, 1, UPPER_CHAN, LOWER_CHAN);

//--------------------------------------------------Pause
    if (Pause.State == 0 && Pause.Prev == 1 && millis() - Pause.current_time > debouncedelay) {
        if (Pause.Output == LOWER_CHAN){
            Pause.Output = UPPER_CHAN;
        }
        else Pause.Output = LOWER_CHAN;
    Pause.current_time = millis();    
    }
    Pause.Prev = Pause.State;
}



void rc_data(){

    channels[0] = Throttle.Output;  //T
    channels[1] = Pitch.Output;     //E
    channels[2] = Roll.Output;      //A
    channels[3] = Yaw.Output;       //R
    channels[4] = Arm.Output;  //mode
    channels[5] = RightPot.Output;   //pot
    channels[6] = Pre.Output;       //Arm
    channels[7] = RTH.Output;       //Pre
    channels[8] = LeftPot.Output;       //RTH
    channels[9] = Pre.Output;     //Pause    
    channels[10]= 1024;
    channels[11]= 1024;
    channels[12]= 1024;
    channels[13]= 1024;
    channels[14]= 1024;
    channels[15]= 1024;
}
