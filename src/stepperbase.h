#pragma once

#include "Arduino.h"
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

        void startMoveTo(int32_t s_tgt, int32_t v_e, uint32_t v_max, uint32_t a);
        void startRotate(int32_t v_max, uint32_t a);
        void startStopping(int32_t va_end, uint32_t a);

        inline void setDir(int d);
        int32_t dir;
        int32_t vDir;

        volatile int32_t pos;
        volatile int32_t target;

        int32_t s_tgt;
        int32_t v_tgt;
        int64_t v_tgt_sqr;

        int32_t twoA;
        int32_t decStart, accEnd;

        volatile int32_t s;
        volatile int32_t v;
        volatile int64_t v_sqr;

        inline void doStep();

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

    void StepperBase::doStep()
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

    void StepperBase::stepISR()
    {
        if (s < accEnd) // accelerating
        {
            v = signum(v_sqr) * sqrtf(std::abs(v_sqr));
            v_sqr += twoA;
            stpTimer->updateFrequency(std::abs(v));
            doStep();
        } else if (s < decStart) // constant speed
        {
            v = std::min(sqrtf(v_sqr), sqrtf(v_tgt_sqr));
            stpTimer->updateFrequency(v);
            doStep();
        } else if (s < s_tgt) // decelerating
        {
            v_sqr -= twoA;
            v = signum(v_sqr) * sqrtf(std::abs(v_sqr));
            stpTimer->updateFrequency(std::abs(v));
            doStep();
        } else // target reached
        {
            stpTimer->stop();
            TimerFactory::returnTimer(stpTimer);
            stpTimer = nullptr;
            isMoving = false;
        }
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
            doStep();
        } else
        {
           // SerialUSB1.println("rotISR reached");
            dir = signum(v_sqr);
            digitalWriteFast(dirPin, dir > 0 ? HIGH : LOW);
            delayMicroseconds(5);

            if (v_tgt != 0)
            {
              //  SerialUSB1.printf("vtgt % d\n",v_tgt);
                stpTimer->updateFrequency(std::abs(v_tgt));
                doStep();
            } else
            {
                //SerialUSB1.printf("rotISR %s stopped\n", name.c_str());
                stpTimer->stop();
                TimerFactory::returnTimer(stpTimer);
                stpTimer = nullptr;
                isMoving = false;
                v_sqr = 0;
            }
        }

        // {
        //     //      Serial.println("rot0");
        //     int32_t v_abs;
        //     if (vDir * (v_tgt_sqr - v_sqr) > twoA)
        //     {
        //         v_sqr += vDir * twoA;

        //         digitalWriteFast(dirPin, signum(v_sqr) > 0 ? HIGH : LOW);
        //         delayMicroseconds(5);

        //         v_abs = sqrtf(std::abs(v_sqr));
        //         stpTimer->updateFrequency(v_abs);
        //         doStep();
        //     } else
        //     {
        //         //      Serial.println("rot2");
        //         v_abs = sqrtf(std::abs(v_tgt_sqr));
        //         v = signum(v_tgt_sqr) * v_abs;

        //         if (mode != mode_t::stopping)
        //         {
        //             stpTimer->updateFrequency(v_abs);
        //             doStep();
        //         } else
        //         {
        //             stpTimer->stop();
        //             TimerFactory::returnTimer(stpTimer);
        //             stpTimer = nullptr;
        //             isMoving = false;
        //             v = 0;
        //             v_sqr = 0;
        //             //       Serial.println("stopped");
        //         }
        //     }

        //     pos += signum(v);
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
}
