#pragma once

#include "interfaces.h"

namespace TS4
{
    namespace TimerFactory
    {
        extern void attachModule(ITimerModule*);
        extern ITimer* makeTimer();
        extern void returnTimer(  ITimer* timer);
    }
}