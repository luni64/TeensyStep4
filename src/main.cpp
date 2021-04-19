#include "Arduino.h"
#include "TS4/teensystep4.h"
#include "TS4/timers/interfaces.h"

using namespace TS4;

Stepper s1(0, 1);
Stepper s2(5, 6);
Stepper s3(7, 8);

// struct TT
// {
//     TT(Stepper& x, Stepper& y)
//         : sX(x), sY(y)
//     {}

//     void moveTo(int x, int y)
//     {
//         sX.setTargetAbs(x);
//         sY.setTargetAbs(y);
//         transport.moveAsyc();
//     }

//     Stepper &sX, sY;
//     StepperGroup transport{sX, sY};
// };

void pr()
{
    static int old1 = 0;
    static int old2 = 0;
    int newP1       = s1.getPosition();
    int newP2       = s2.getPosition();
    if (newP1 != old1 || newP2 != old2)
    {
        Serial.printf("%d %d\n", newP1, newP2);
        old1 = newP1;
        old2 = newP2;
    }
}

void setup()
{
    for (int pin : {LED_BUILTIN, 0, 1, 2, 14}) { pinMode(pin, OUTPUT); }
    while (!Serial) {}

    TS4::attachErrFunc(ErrorHandler(SerialUSB1));

    TS4::begin();



    s1
        .setMaxSpeed(15001)
        .setAcceleration(50'000)
        .name = "s1";

    s2
        .setMaxSpeed(15'000)
        .setAcceleration(50'000)
        .name = "s2";

    s3
        .setMaxSpeed(15'000)
        .setAcceleration(50'000)
        .name = "s3";
    //        .setTargetAbs(12030);

    //    TT t1(s1, s2);


    // StepperGroup g;10

    StepperGroup g  {s1, s2};
    s1.setAcceleration(20000);
    s1.setMaxSpeed(500);
    s2.setMaxSpeed(2000);
    //g.move({1000,20});
    s1.moveAbsAsync(20);
    // delay(500);
    // s1.moveAbsAsync(3000,400);

    // delay(2000);1
    // g.stopAsync();

    //g.move();

    //delay(3);

//    Serial.printf("done %d %d %d\n", s1.getPosition(), s2.getPosition(), s3.getPosition());

//     // s1.setTargetAbs(1000);
//     // s1.moveAsync();

//     s1.setTargetAbs(-1600);
//     s2.setTargetAbs(-10000);
//     s3.setTargetAbs(-300);
//     g.move();
// Serial.printf("done %d %d %d\n", s1.getPosition(), s2.getPosition(), s3.getPosition());
//     // transport.moveAsyc();

    // StepperGroup xyTrans{s2, s2, s1};

    // Stepper* sa[] = {&s1, &s1, &s2, nullptr};

    // Serial.printf("%p %p\n", &s1, &s2);
    //StepperGroup sg{new Stepper(1,2), new Stepper(3,4)};

    //g.moveAsyc();
    //(new IntervalTimer)->begin(pr, 10'000);
}

elapsedMillis sw;
uint32_t old = 0;

IMXRT_TMR_CH_t* pCH0 = &IMXRT_TMR4.CH[0];

void loop()
{
    if (Serial.available())
    {
        static char oldChar = 'x';
        char newChar        = Serial.read();
        if (newChar == oldChar) return;

        oldChar = newChar;

        int t;

        switch (newChar)
        {
            case '4':
                Serial.println("l");
                s1.rotateAsync(10000);
                break;
            case '6':
                Serial.println("r");
                s1.rotateAsync(-10000);
                break;
            case ' ':
                s1.stopAsync();
                break;

            case '8':
                t = random(-10000, 10000);
                Serial.printf("r %d\n", t);
                s1.moveRel(t);
                break;
            case '2':
                Serial.println("n");
                s1.moveRel(-30000);
                break;

            default:
                //Serial.println((int)newChar);
                break;
        }
    }

    if (sw >= 200)
    {
        sw -= 200;

        //Serial.printf("%u %d\n", millis(), s.getPosition());

        //Serial.println(pCH0->CNTR);

        digitalToggleFast(LED_BUILTIN);
    }

    // if(p == 123.0f)
    // {
    //     p = 500.0f;
    // }
    // else
    // {
    //     p = 123.0f;
    // }
    // digitalWriteFast(1, HIGH);
    // t1.setCurrentPeriod(p);
    // digitalWriteFast(1, LOW);

    //delay(100);
}
