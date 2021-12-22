#include "Arduino.h"
#include "TS5/PulseGenerator.h"

PulseGenerator pg(11);

void setup()
{
     asm(".global _printf_float");
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

    if (sw >= 20)
    {
        sw -= 20;
        digitalToggleFast(LED_BUILTIN);
    }
}
