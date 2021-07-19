#include "Arduino.h"
#include "teensystep4.h"
using namespace TS4;

Stepper s1(0, 2);

void setup()
{
    TS4::begin();

    s1.setMaxSpeed(10'000);
    s1.setAcceleration(50'000);

    s1.moveAbs(1000);
}

void loop()
{
    s1.moveRel(-500);
    delay(200);

    s1.moveRel(500);
    delay(200);
}