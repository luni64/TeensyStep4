#pragma once

#include "Arduino.h"
#include "ErrorHandling/error_handler.h"
#include "timers/interfaces.h"
#include "timers/timerfactory.h"
#include <algorithm>
#include <cstdint>

namespace TS4
{
    template <typename T>
    int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    class StepperBase
    {
     public:
        std::string name;
        bool isMoving = false;

        const int stepPin, dirPin;
        StepperBase(const uint stepPin, const uint dirPin, const uint pulseWidth = 10);
     protected:

        errorCode doMoveAsync(int32_t s_tgt, uint32_t v_s, uint32_t v_e, uint32_t v_max, uint32_t a);
        void doRotateAsync(int32_t v_max, uint32_t a);
        void doStopAsync(int32_t va_end, uint32_t a);

        int64_t tgtS = 11;
        int64_t tgtV = 0;

        volatile int64_t curS;
        volatile int64_t curV;

        // inline void setDir(int d);
        // int32_t dir;
        // int32_t vDir;

        // volatile int32_t pos, target;

        // int32_t s_tgt;
        // int32_t v_tgt;
        // int64_t v_tgt_sqr;

        // int32_t twoA, decStart, accEnd, accLen;

        // volatile int32_t s;
        // volatile int32_t v;
        // volatile int64_t v_sqr;

        // inline void makeStep();


        IPinTimer* const stepTimer;
      
        enum class mode_t {
            target,
            rotate,
            stopping,
        } mode = mode_t::target;

        // Bresenham:
        StepperBase* next = nullptr; // linked list of steppers, maintained from outside
        int32_t A, B;                // Bresenham parameters (https://en.wikipedia.org/wiki/Bresenham)

        friend class StepperGroupBase;

        inline float doStep();
    };

    //========================================================================================================
    // Inline implementation
    //========================================================================================================

    // void StepperBase::stepISR()
    // {
    // }

    // void StepperBase::makeStep()
    // {
    // }

    float StepperBase::doStep()
    {
        int64_t dS = tgtS - curS;
        int64_t dV = tgtV - curV;

        int cur_dir = sgn(curV);
        int tgt_dir = sgn(tgtV);
        int delta;

        if (cur_dir == 0) // need to treat this specially
        {
            if (tgt_dir == 0 && std::abs(dS) == 1) // v_cur=0, v_tgt=0 and one step appart -> dummy move with v=0 (works since we will limit to vs later)
            {
                curS += sgn(dS);
            }
            else
            {
                delta   = dS - dV;
                int dir = delta == 0 ? sgn(dS) : sgn(delta);
                curS += dir;
                curV += dir;
            }
            //Serial.printf("S:%d, V:%d  v:%.2f\n", (int)curS, (int)curV, 10000.0f/sqrtf(curV));
            return (uint16_t)(10000.0f / sqrtf(curV));
        }

        delta = cur_dir * dS - std::abs(dV);                    // delta < 0 -> can not reach directly, delta == 0 -> '45°' delta==1 -> keep v, delta > 1 increase v
        if ((tgt_dir == cur_dir || tgt_dir == 0) && delta >= 0) // we can reach target directly (without change of direction)
        {
            //
            if (delta == 0 || (delta == 1 && cur_dir * dV >= 0)) //
            {                                                    //
                curS += cur_dir;                                 //
                curV += sgn(dV);                                 //
                                                                 //
            }                                                    //
            else if (delta > 1)                                  //
            {                                                    //
                curS += cur_dir;                                 //
                curV += cur_dir;                                 //
                                                                 //
            }                                                    //
            else                                                 // delta == 1
            {                                                    //
                curS += cur_dir;                                 //
            }                                                    //
        }                                                        //
        else                                                     // need to swich direction -> reduce until 0
        {                                                        //
            int s_cur_cross = curS + curV;
            int s_tgt_cross = tgtS - tgtV;
            tgtS            = cur_dir * std::max(cur_dir * s_tgt_cross, cur_dir * s_cur_cross);
            dS              = tgtS - curS;
            dV              = -curV;
            delta           = std::abs(dS) - std::abs(dV);

            if (delta == 0 || (delta == 1 && cur_dir * dV >= 0))
            {
                curS += cur_dir;
                curV += sgn(dV);
            }
            else if (delta > 1)
            {
                curS += cur_dir;
                curV += cur_dir;
            }
            else
            {
                curS += cur_dir;
                curV += 0;
            }
        }
        // Serial.printf("S:%d, V:%d  v:%.2f\n", (int)curS, (int)curV, 10000.0f/sqrtf(curV));
        return (uint16_t)(10000.0f / sqrtf(curV));
    }
}
