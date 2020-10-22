void setup_left_mlx(){
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

void setup_right_mlx(){
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

void loop_left_mlx(){
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

void loop_right_mlx(){
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

void ComputeRC3(){
//--------------------------------------------------Throttle
    if (Throttle.Reading > 50000){
        Throttle.Inter = Throttle.Reading - 65535;
    }
    else Throttle.Inter = Throttle.Reading;
    Throttle.Output = map(Throttle.Inter, -10000, 10000, -100, 100);

//--------------------------------------------------Yaw
    if (Yaw.Reading > 50000){
        Yaw.Inter = Yaw.Reading - 65535;
    }
    else Yaw.Inter = Yaw.Reading;
    Yaw.Output = map(Yaw.Inter, -10000, 10000, -100, 100);
//--------------------------------------------------Pitch
    if (Pitch.Reading > 50000){
        Pitch.Inter = Pitch.Reading - 65535;
    }
    else Pitch.Inter = Pitch.Reading;
    Pitch.Output = map(Pitch.Inter, -10000, 10000, -100, 100);
//--------------------------------------------------Roll
    if( Roll.Reading > 50000){
        Roll.Inter = Roll.Reading - 65535;
    }
    else Roll.Inter = Roll.Reading;
    Roll.Output = map(Roll.Inter, -10000, 10000, -100, 100);

//--------------------------------------------------Right Pot
    RightPot.Output = map(RightPot.State, 0, 4095, 100, -100);
    RightPot.Output = constrain(RightPot.Output, -100, 100);

//--------------------------------------------------Left Pot
    LeftPot.Output = map(LeftPot.State, 3570, 440, -100, 100);
    LeftPot.Output = constrain(LeftPot.Output, -100, 100);

    LeftPot.Process = map(LeftPot.State, 3570, 440, -100, 100);
    currenttime = millis();
    if (LeftPot.Process > 75 && millis() - currenttime > debouncedelay && LeftPot.Output < 80){
        LeftPot.Output += 40;
    }
    currenttime = millis();
    if (LeftPot.Process > -75 && millis() - currenttime > debouncedelay && LeftPot.Output > -80){
        LeftPot.Output -= 40;
    }
    currenttime = millis();

//--------------------------------------------------Arm
    if (Arm.State == 0 && Arm.Prev == 1 && millis() - currenttime > debouncedelay) {
        if (Arm.Output == -100){
            Arm.Output = 100;
        }
        else Arm.Output = -100;
    }

    Arm.Prev = Arm.State;

//--------------------------------------------------RTH
    if (RTH.State == 0 && RTH.Prev == 1 && millis() - currenttime > debouncedelay) {
        if (RTH.Output == -100){
            RTH.Output = 100;
        }
        else RTH.Output = -100;
    currenttime = millis();    
    }
    RTH.Prev = RTH.State;
//--------------------------------------------------Pre
    Pre.Output = map(Pre.State, 0, 1, 100, -100);

//--------------------------------------------------Pause
    if (Pause.State == 0 && Pause.Prev == 1 && millis() - currenttime > debouncedelay) {
        if (Pause.Output == -100){
            Pause.Output = 100;
        }
        else Pause.Output = -100;
    currenttime = millis();    
    }
    Pause.Prev = Pause.State;
}

void compute_rc4(){
//--------------------------------------------------Throttle

    Throttle.Output = constrain(map(Throttle.Reading, -7600, 8300, -100, 100), -100, 100);

//--------------------------------------------------Yaw

    Yaw.Output = constrain(map(Yaw.Reading, -7500, 9200, -100, 100), -100, 100);

//--------------------------------------------------Pitch

    Pitch.Output = constrain(map(Pitch.Reading, -7300, 8520, -100, 100), -100, 100);

//--------------------------------------------------Roll

    Roll.Output = constrain(map(Roll.Reading, -8300, 8400, -100, 100), -100, 100);

//--------------------------------------------------Right Pot
    LeftPot.Output = map(LeftPot.State, 440, 3600, 100, -100);
    LeftPot.Output = constrain(LeftPot.Output, -100, 100);

//--------------------------------------------------Left Pot
    RightPot.Output = map(RightPot.State, 3570, 440, -100, 100);
    RightPot.Output = constrain(RightPot.Output, -100, 100);

    RightPot.Process = map(RightPot.State, 3570, 440, -100, 100);
    RightPot.current_time = millis();
    if (RightPot.Process > 75 && millis() - RightPot.current_time > debouncedelay && RightPot.Output < 80){
        RightPot.Output += 40;
    }
    RightPot.current_time = millis();
    if (RightPot.Process > -75 && millis() - RightPot.current_time > debouncedelay && RightPot.Output > -80){
        RightPot.Output -= 40;
    }

    RightPot.current_time = millis();

//--------------------------------------------------Arm
    if (Arm.State == 0 && Arm.Prev == 1 && millis() - Arm.current_time > debouncedelay) {
        if (Arm.Output == -100){
            Arm.Output = 100;
        }
        else Arm.Output = -100;
        Arm.current_time = millis();
    }

    Arm.Prev = Arm.State;
/*
//--------------------------------------------------RTH
    if (RTH.State == 0 && RTH.Prev == 1 && millis() - currenttime > debouncedelay) {
        if (RTH.Output == -100){
            RTH.Output = 100;
        }
        else RTH.Output = -100;
    currenttime = millis();    
    }
    RTH.Prev = RTH.State;
*/
//--------------------------------------------------RTH
    RTH.State = digitalRead(RTH.Pin);
    if (RTH.State == 0 && RTH.Prev == 1 && millis() - RTH.current_time > debouncedelay) {
        if (RTH.Output == -100){
            RTH.Output = 100;
        }
        else RTH.Output = -100;
    RTH.current_time = millis();    
    }
    RTH.Prev = RTH.State;

//--------------------------------------------------Pre
    Pre.Output = map(Pre.State, 0, 1, 100, -100);

//--------------------------------------------------Pause
    if (Pause.State == 0 && Pause.Prev == 1 && millis() - Pause.current_time > debouncedelay) {
        if (Pause.Output == -100){
            Pause.Output = 100;
        }
        else Pause.Output = -100;
    Pause.current_time = millis();    
    }
    Pause.Prev = Pause.State;
}



void rc_data(){

    channels[0] = Throttle.Output;  //T
    channels[1] = Pitch.Output;     //E
    channels[2] = Roll.Output;      //A
    channels[3] = Yaw.Output;       //R
    channels[4] = RightPot.Output;  //mode
    channels[5] = LeftPot.Output;   //pot
    channels[6] = Arm.Output;       //Arm
    channels[7] = Pre.Output;       //Pre
    channels[8] = RTH.Output;       //RTH
    channels[9] = Pause.Output;     //Pause    
    
}
