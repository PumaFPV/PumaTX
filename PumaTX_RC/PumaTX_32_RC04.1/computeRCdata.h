void compute_rc()
{
//--------------------------------------------------throttle

    throttle.output = constrain(map(throttle.reading, -7950, 9150, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------yaw

    yaw.output = constrain(map(yaw.reading, -8250, 9530, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------pitch

    pitch.output = constrain(map(pitch.reading, -7640, 8540, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------roll

    roll.output = constrain(map(roll.reading, -8920, 8200, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Right Pot
    rightpot.output = constrain(map(rightpot.state, 440, 3600, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Left Pot

    leftpot.process = constrain(map(leftpot.state, 3570, 440, -100, 100), -100, 100);
    
    if (leftpot.process > 75 && millis() - leftpot.current_time > debouncedelay && leftpot.intermediate <= 80)
    {
        leftpot.intermediate += 40;
        leftpot.current_time = millis();

    }
    //leftpot.current_time = millis();
    if (leftpot.process < -75 && millis() - leftpot.current_time > debouncedelay && leftpot.intermediate >= -80)
    {
        leftpot.intermediate -= 40;
        leftpot.current_time = millis();

    }

    leftpot.output = map(constrain(leftpot.intermediate, -100, 100), -100, 100, LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------arm
    if (arm.state == 0 && arm.prev == 1 && millis() - arm.current_time > debouncedelay)
    {
        if (arm.output == LOWER_CHAN)
        {
            arm.output = UPPER_CHAN;
        }
        else arm.output = LOWER_CHAN;
        
        arm.current_time = millis();
    }

    arm.prev = arm.state;

//--------------------------------------------------rth
    if (rth.state == 0 && rth.prev == 1 && millis() - rth.current_time > debouncedelay)
    {
        if (rth.output == LOWER_CHAN)
        {
            rth.output = UPPER_CHAN;
        }
        else rth.output = LOWER_CHAN;
        
    rth.current_time = millis();    
    }
    
    rth.prev = rth.state;

//--------------------------------------------------pre
    pre.output = map(pre.state, 0, 1, UPPER_CHAN, LOWER_CHAN);

//--------------------------------------------------play
    if (play.state == 0 && play.prev == 1 && millis() - play.current_time > debouncedelay)
    {
        if (play.output == LOWER_CHAN){
            play.output = UPPER_CHAN;
        }
        else play.output = LOWER_CHAN;
        
    play.current_time = millis();    
    }
    
    play.prev = play.state;
    
}



void rc_data()
{

    channels[0] = throttle.output;  //T
    channels[1] = pitch.output;     //E
    channels[2] = roll.output;      //A
    channels[3] = yaw.output;       //R
    channels[4] = arm.output;  //mode
    channels[5] = rightpot.output;   //pot
    channels[6] = pre.output;       //arm
    channels[7] = rth.output;       //pre
    channels[8] = leftpot.output;       //rth
    channels[9] = pre.output;     //play    
    channels[10]= 1024;
    channels[11]= 1024;
    channels[12]= 1024;
    channels[13]= 1024;
    channels[14]= 1024;
    channels[15]= 1024;
}

void get_mlx_data()
{
  
  throttle.reading = mlx.get_throttle();
  yaw.reading = mlx.get_yaw();
  pitch.reading = mlx.get_pitch();
  roll.reading = mlx.get_roll();
  
}  
