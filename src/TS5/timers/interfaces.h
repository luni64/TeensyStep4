#pragma once
#include "Arduino.h"
#include <memory>

namespace TS4
{
    // constexpr int32_t signum(int64_t v)
    // {
    //     return (0 < v) - (v < 0);
    // }

    class StepperBase;

    // using callback_t = float (StepperBase::*)();

    // Implement this interface for the timers you want to use
    class IPinTimer
    {
     public:
        virtual void begin(unsigned pin, float pulseWidth) = 0;
        virtual void start()                               = 0;
        virtual void stop()                                = 0;
        void attachStepcallback(StepperBase* stepper){this->stepper = stepper;};

     
        StepperBase* stepper;

        virtual ~IPinTimer() {}

     protected:
    };

    //==============================================================
    // A TimerModule has one or more timer channels.
    //

    class ITimerModule
    {
     public:
        virtual IPinTimer* getChannel()         = 0;
        virtual void releaseChannel(IPinTimer*) = 0;
        virtual size_t getFreeChannels()        = 0;
    };
}