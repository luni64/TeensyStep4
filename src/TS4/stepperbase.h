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
        if (s != s_tgt)
        {
            stpTimer->up dateFrequency(sqrtf(std::abs((v_sqr))));
            makeStep();
        }


        int32_t ds    = s_tgt - pos;
        int32_t _twoA = twoA * signum(ds);

        int32_t accLen = std::abs(v_sqr / twoA);
        //if (accLen > std::abs(ds / 2)) accLen = std::abs(ds / 2);

        SerialUSB1.printf("ds: %d , twoA: %d, accLen: %d\n", ds, _twoA, accLen);
         SerialUSB1.printf("vsqr: %.0f \n", (float)v_sqr);

        if (ds > accLen)
            v_sqr += twoA;
        else
            v_sqr -= twoA;
        SerialUSB1.printf("vsqr2: %.0f \n", (float)v_sqr);

        dir = signum(v_sqr);
        digitalWriteFast(dirPin, dir >= 0 ? HIGH : LOW);
        delayMicroseconds(5);


        // if (s < accEnd) // accelerating
        // {
        //     //postError(errorCode::message, "acc");
        //     int32_t v_abs = sqrtf(std::abs(v_sqr));
        //     v             = signum(v_sqr) * v_abs;
        //     v_sqr += twoA;
        //     stpTimer->updateFrequency(v_abs);
        //     makeStep();
        // }
        // else if (s < decStart) // constant speed
        // {
        //     //postError(errorCode::message, "const");
        //     //v = std::min(sqrtf(v_sqr), sqrtf(v_tgt_sqr));
        //     v = sqrtf(v_sqr);

        //     stpTimer->updateFrequency(v);
        //     makeStep();
        // }
        // else if (s < s_tgt) // decelerating
        // {
        //     //postError(errorCode::message, "dec");
        //     v_sqr -= twoA;
        //     v = signum(v_sqr) * sqrtf(std::abs(v_sqr));
        //     stpTimer->updateFrequency(std::abs(v));
        //     makeStep();
        // }
        // else // target reached
        // {
        //     stpTimer->stop();
        //     TimerFactory::returnTimer(stpTimer);
        //     stpTimer = nullptr;
        //     isMoving = false;
        // }
    }

    void StepperBase::rotISR()
    {
        int32_t v_abs;

        if (std::abs(v_sqr - v_tgt_sqr) > twoA) // target speed not yet reached
        {
            v_sqr += vDir * twoA;

            dir = signum(v_sqr);
            digitalWriteFast(dirPin, dir > 0 ? HIGH : LOW);
            delayMicroseconds(5);

            v_abs = sqrtf(std::abs(v_sqr));
            stpTimer->updateFrequency(v_abs);
            makeStep();
        }
        else
        {
            // SerialUSB1.println("rotISR reached");
            dir = signum(v_sqr);
            digitalWriteFast(dirPin, dir > 0 ? HIGH : LOW);
            delayMicroseconds(5);

            if (v_tgt != 0)
            {
                //  SerialUSB1.printf("vtgt % d\n",v_tgt);
                stpTimer->updateFrequency(std::abs(v_tgt));
                makeStep();
            }
            else
            {
                postError(errorCode::message, "Stopped");
                stpTimer->stop();
                TimerFactory::returnTimer(stpTimer);
                stpTimer = nullptr;
                isMoving = false;
                v_sqr    = 0;
            }
        }
    }

    void StepperBase::resetISR()
    {
        // Serial.println("r");
        // Serial.flush();
        StepperBase* stepper = this;
        while (stepper != nullptr)
        {
            digitalWriteFast(stepper->stepPin, LOW);
            stepper = stepper->next;
        }
    }

    void StepperBase::makeStep()
    {
        digitalWriteFast(stepPin, HIGH);
        s += 1;
        pos += dir;

        StepperBase* stepper = next;
        while (stepper != nullptr) // move slave motors if required
        {
            if (stepper->B >= 0)
            {
                digitalWriteFast(stepper->stepPin, HIGH);
                stepper->pos += stepper->dir;
                stepper->B -= this->A;
            }
            stepper->B += stepper->A;
            stepper = stepper->next;
        }
    }
}
