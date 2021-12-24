#pragma once

#include "Arduino.h"
#include "ErrorHandling/error_handler.h"
#include "timers/interfaces.h"
#include "timers/timerfactory.h"
#include <algorithm>
#include <cstdint>

namespace TS4
{
    class StepperBase
    {
     public:
        std::string name;
        bool isMoving = false;

     protected:
        StepperBase(const int stepPin, const int dirPin);

        errorCode doMoveAsync(int32_t s_tgt, uint32_t v_s, uint32_t v_e, uint32_t v_max, uint32_t a);
        void doRotateAsync(int32_t v_max, uint32_t a);
        void doStopAsync(int32_t va_end, uint32_t a);

        inline void setDir(int d);
        int32_t dir;
        int32_t vDir;

        volatile int32_t pos, target;

        int32_t s_tgt;
        int32_t v_tgt;
        int64_t v_tgt_sqr;

        int32_t twoA, decStart, accEnd, accLen;

        volatile int32_t s;
        volatile int32_t v;
        volatile int64_t v_sqr;

        inline void makeStep();

        const int stepPin, dirPin;

        ITimer* stpTimer;
        inline void stepISR();
        inline void rotISR();
        inline void resetISR();

        enum class mode_t {
            target,
            rotate,
            stopping,
        } mode = mode_t::target;

        // Bresenham:
        StepperBase* next = nullptr; // linked list of steppers, maintained from outside
        int32_t A, B;                // Bresenham parameters (https://en.wikipedia.org/wiki/Bresenham)

        friend class StepperGroupBase;
    };

    //========================================================================================================
    // Inline implementation
    //========================================================================================================

    void StepperBase::stepISR()
    {
    }

    void StepperBase::makeStep()
    {
    }
}
