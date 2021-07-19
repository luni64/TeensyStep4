#include "Arduino.h"
#include "teensystep4.h"

using namespace TS4;

Stepper s1(0, 3);
Stepper s2(1, 4);
Stepper s3(2, 5);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    TS4::begin();

    s1
        .setMaxSpeed(10'000)
        .setAcceleration(50'000);

    s2
        .setMaxSpeed(8'000)
        .setAcceleration(25'000);

    s3
        .setMaxSpeed(20'000)
        .setAcceleration(50'000);

    // move two steppers simultaneously (not synchronized)
    s1.moveAbsAsync(1000);
    s2.moveAbsAsync(4000);

    // wait until both steppers are done
    while (s1.isMoving || s2.isMoving)
    {
        delay(10);
    }
    delay(100);

    // define and move a groups of steppers synchronized
    StepperGroup g1{s1, s2};
    StepperGroup g2{s2, s3};

    s1.setTargetAbs(-1000);
    s2.setTargetAbs(-3000);
    g1.move();
    delay(100);

    s2.setTargetAbs(1000);
    s3.setTargetAbs(4000);
    g2.move();
    delay(100);

    // define and move a stepper groups on the fly
    s3.setTargetAbs(-2000);
    s1.setTargetAbs(-3000);
    StepperGroup{s3, s1}.move();

    delay(100);

    s1.setTargetAbs(0);
    s2.setTargetAbs(0);
    s3.setTargetAbs(0);
    StepperGroup({s1, s2, s3}).move();
}

void loop()
{
    digitalToggleFast(LED_BUILTIN);
    delay(200);
}