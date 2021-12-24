#pragma once

#include "interfaces.h"
#include <memory>


namespace TS4
{
    namespace TimerFactory
    {
        extern ITimer* makeTimer(unsigned pin);
        extern void returnTimer(  ITimer* timer);
    }
}