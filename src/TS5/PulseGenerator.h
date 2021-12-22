#pragma once
#include "Arduino.h"
#include "CallbackStateHelper.h"

#include <cstdint>
class PulseGenerator;
using pG_state_t    = IMXRT_TMR_CH_t*;
using pG_callback_t = void(pG_state_t);

class PulseGenerator: public CallbackStateHelper<PulseGenerator, pG_callback_t, pG_state_t, 4>
{
 public:
    PulseGenerator(uint8_t pin);

    void begin();
    void trigger();

    void beginTMR();

    IMXRT_TMR_CH_t* regs;
    const uint8_t pin;

    static void isr(pG_state_t THIS);
};