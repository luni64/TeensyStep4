#include "Arduino.h"
#include "TS5/PulseGenerator.h"
#include "TS5/timers/timerfactory.h"
#include "TS5/log.h"

using namespace TS4;

PulseGenerator pg(11);

auto* t = TimerFactory::makeTimer(11);

void setup()
{
    while (!Serial) {}
    asm(".global _printf_float");

    t->start();

    LOG("Start");




    for (int pin : {LED_BUILTIN, 0, 1, 2, 11}) { pinMode(pin, OUTPUT); }

    while (!Serial) {}

    pg.beginTMR();
    digitalWriteFast(1, LOW);
}

elapsedMillis sw = 500;
uint32_t old     = 0;

// IMXRT_TMR_CH_t* pCH0 = &IMXRT_TMR4.CH[0];

extern float res;

int64_t oldS = 0, oldV = 0;
extern volatile int64_t curS, curV;
extern volatile int64_t tgtS, tgtV;

void loop()
{
    if (oldS != curS && curV != tgtV)
    {

        oldS = curS;
        oldV = curV;
    }

    if (sw >= 200)
    {
        sw -= 200;
        digitalToggleFast(LED_BUILTIN);
    }
}
