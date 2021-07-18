#pragma once
#include "Arduino.h"
#include <functional>
#include <vector>

namespace TS4
{
    constexpr int32_t signum(int64_t v)
    {
        return (0 < v) - (v < 0);
    }

    using callback_t = std::function<void(void)>;

    // Implement this interface for the timers you want to use
    class ITimer
    {
     public:
        virtual void setPulseParams(float width, unsigned pin)              = 0;
        virtual void attachCallbacks(callback_t stepCb, callback_t resetCb) = 0;
        virtual void updateFrequency(float f)                               = 0;
        virtual void start()                                                = 0;
        virtual void stop()                                                 = 0;

        virtual ~ITimer() {}

     protected:
    };

    //==============================================================
    // A TimerModule has one or more timer channels.
    //

    class ITimerModule
    {
     public:
        virtual ITimer* getChannel()         = 0;
        virtual void releaseChannel(ITimer*) = 0;
    };
}