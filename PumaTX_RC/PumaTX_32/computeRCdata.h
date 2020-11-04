

void compute_rc(){
//--------------------------------------------------throttle

    throttle.ouput = constrain(map(throttle.reading, -7600, 8300, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------yaw

    yaw.ouput = constrain(map(yaw.reading, -7500, 9200, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------pitch

    pitch.ouput = constrain(map(pitch.reading, -7300, 8520, LOWER_CHAN, UPPER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------roll

    roll.ouput = constrain(map(roll.reading, -8300, 8400, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Right Pot
    rightpot.ouput = constrain(map(rightpot.state, 440, 3600, UPPER_CHAN, LOWER_CHAN), LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------Left Pot

    leftpot.process = constrain(map(leftpot.state, 3570, 440, -100, 100), -100, 100);
    
    if (leftpot.process > 75 && millis() - leftpot.current_time > debouncedelay && leftpot.intermediate <= 80){
        leftpot.intermediate += 40;
        leftpot.current_time = millis();

    }
    //leftpot.current_time = millis();
    if (leftpot.process < -75 && millis() - leftpot.current_time > debouncedelay && leftpot.intermediate >= -80){
        leftpot.intermediate -= 40;
        leftpot.current_time = millis();

    }

    leftpot.ouput = map(constrain(leftpot.intermediate, -100, 100), -100, 100, LOWER_CHAN, UPPER_CHAN);

//--------------------------------------------------arm
    if (arm.state == 0 && arm.prev == 1 && millis() - arm.current_time > debouncedelay) {
        if (arm.ouput == LOWER_CHAN){
            arm.ouput = UPPER_CHAN;
        }
        else arm.ouput = LOWER_CHAN;
        arm.current_time = millis();
    }

    arm.prev = arm.state;

//--------------------------------------------------rth
    if (rth.state == 0 && rth.prev == 1 && millis() - rth.current_time > debouncedelay) {
        if (rth.ouput == LOWER_CHAN){
            rth.ouput = UPPER_CHAN;
        }
        else rth.ouput = LOWER_CHAN;
    rth.current_time = millis();    
    }
    rth.prev = rth.state;

//--------------------------------------------------pre
    pre.ouput = map(pre.state, 0, 1, UPPER_CHAN, LOWER_CHAN);

//--------------------------------------------------play
    if (play.state == 0 && play.prev == 1 && millis() - play.current_time > debouncedelay) {
        if (play.ouput == LOWER_CHAN){
            play.ouput = UPPER_CHAN;
        }
        else play.ouput = LOWER_CHAN;
    play.current_time = millis();    
    }
    play.prev = play.state;
}



void rc_data(){

    channels[0] = throttle.ouput;  //T
    channels[1] = pitch.ouput;     //E
    channels[2] = roll.ouput;      //A
    channels[3] = yaw.ouput;       //R
    channels[4] = arm.ouput;  //mode
    channels[5] = rightpot.ouput;   //pot
    channels[6] = pre.ouput;       //arm
    channels[7] = rth.ouput;       //pre
    channels[8] = leftpot.ouput;       //rth
    channels[9] = pre.ouput;     //play    
    channels[10]= 1024;
    channels[11]= 1024;
    channels[12]= 1024;
    channels[13]= 1024;
    channels[14]= 1024;
    channels[15]= 1024;
}
