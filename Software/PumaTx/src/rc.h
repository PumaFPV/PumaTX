int16_t throttleMin = -7950;
int16_t throttleMax = 9150;
int16_t yawMin = -8250;
int16_t yawMax = 9530;
int16_t pitchMin = -7640;
int16_t pitchMax = 8540;
int16_t rollMin = -8920;
int16_t rollMax = 8200;


void computeRc()
{
//-----Throttle

    throttle.output = constrain(map(throttle.reading, -7950, 9150, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//-----Yaw

    yaw.output = constrain(map(yaw.reading, -8250, 9530, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//-----Pitch

    pitch.output = constrain(map(pitch.reading, -7640, 8540, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//-----Roll

    roll.output = constrain(map(roll.reading, -8920, 8200, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//-----Right Pot
    
    rightPot.output = constrain(map(rightPot.state, 440, 3600, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//-----Left Pot

    leftPot.process = constrain(map(leftPot.state, 3570, 440, -100, 100), -100, 100);
    
    if (leftPot.process > 75 && millis() - leftPot.currentTime > debounceDelay && leftPot.intermediate <= 80)
    {
        leftPot.intermediate += 40;
        leftPot.currentTime = millis();
    }
    if (leftPot.process < -75 && millis() - leftPot.currentTime > debounceDelay && leftPot.intermediate >= -80)
    {
        leftPot.intermediate -= 40;
        leftPot.currentTime = millis();
    }

    leftPot.output = map(constrain(leftPot.intermediate, -100, 100), -100, 100, LOWER_CHAN, UPPER_CHAN);

//-----Arm
    if (arm.state == 0 && arm.prev == 1 && millis() - arm.currentTime > debounceDelay)
    {
        if (arm.output == LOWER_CHAN)
        {
            arm.output = UPPER_CHAN;
        }
        else arm.output = LOWER_CHAN;
        
        arm.currentTime = millis();
    }

    arm.prev = arm.state;

//-----RTH
    if (rth.state == 0 && rth.prev == 1 && millis() - rth.currentTime > debounceDelay)
    {
        if (rth.output == LOWER_CHAN)
        {
            rth.output = UPPER_CHAN;
        }
        else rth.output = LOWER_CHAN;
        
    rth.currentTime = millis();    
    }
    
    rth.prev = rth.state;

//-----Pre
    pre.output = map(pre.state, 0, 1, UPPER_CHAN, LOWER_CHAN);

//-----Play
    if (play.state == 0 && play.prev == 1 && millis() - play.currentTime > debounceDelay)
    {
        if (play.output == LOWER_CHAN){
            play.output = UPPER_CHAN;
        }
        else play.output = LOWER_CHAN;
        
    play.currentTime = millis();    
    }
    
    play.prev = play.state;
    
//-----C1
    if (c1.state == 0 && c1.prev == 1 && millis() - c1.currentTime > debounceDelay)
    {
        if (c1.output == LOWER_CHAN){
            c1.output = UPPER_CHAN;
        }
        else c1.output = LOWER_CHAN;
        
    c1.currentTime = millis();    
    }
    
    c1.prev = c1.state;
    
//-----C2
    if (c2.state == 0 && c2.prev == 1 && millis() - c2.currentTime > debounceDelay)
    {
        if (c2.output == LOWER_CHAN){
            c2.output = UPPER_CHAN;
        }
        else c2.output = LOWER_CHAN;
        
    c2.currentTime = millis();    
    }
    
    c2.prev = c2.state;
    
}

void rcData()
{

    channels[0] = throttle.output;  //T
    channels[1] = pitch.output;     //E
    channels[2] = roll.output;      //A
    channels[3] = yaw.output;       //R
    channels[4] = arm.output;       //mode
    channels[5] = rightPot.output;  //pot
    channels[6] = pre.output;       //arm
    channels[7] = rth.output;       //pre
    channels[8] = leftPot.output;   //rth
    channels[9] = pre.output;       //play    
    channels[10]= c1.output;
    channels[11]= c2.output;
    channels[12]= 1024;
    channels[13]= 1024;
    channels[14]= 1024;
    channels[15]= 1024;
}

void getMlxData()
{
    throttle.reading = mlx.getThrottle();
    yaw.reading =      mlx.getYaw();
    pitch.reading =    mlx.getPitch();
    roll.reading =     mlx.getRoll();
}  
