#pragma once

#include "interfaces.h"
#include <memory>

namespace TS4
{
    class StepperBase;
    namespace TimerFactory
    {
        extern IPinTimer* makeTimer( StepperBase* stepper);
        extern void returnTimer(IPinTimer* timer);
    }
}