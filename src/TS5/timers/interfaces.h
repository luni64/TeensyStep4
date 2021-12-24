#pragma once
#include "Arduino.h"

#include <memory>

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
        virtual void begin(unsigned pin, float pulseWidth)                  = 0;
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
        virtual size_t getFreeChannels()     = 0;
    };
}