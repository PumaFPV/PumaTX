#include <Arduino.h>
#include "variables.h"


void hapticSetup()
{
    ledcSetup(1, 40000, 8);
    ledcAttachPin(HAPTIC_PWM, 1);
    ledcWrite(1, 200);
    delay(400);
    ledcWrite(1, 0);
}

void hapticHandler()
{
    ledcWrite(1, hapticDuty);
}