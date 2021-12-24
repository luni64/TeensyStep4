#include "stepperbase.h"
#include <algorithm>

namespace TS4
{
    StepperBase::StepperBase(int _stepPin, int _dirPin)
        : s(0), v(0), v_sqr(0), stepPin(_stepPin), dirPin(_dirPin)
    {
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
    }

    void StepperBase::doRotateAsync(int32_t _v_tgt, uint32_t a)
    {
        v_tgt     = _v_tgt;
        v_tgt_sqr = (int64_t)signum(v_tgt) * v_tgt * v_tgt;
        vDir      = (int32_t)signum(v_tgt_sqr - v_sqr);
        twoA      = 2 * a;

        // Serial.println("\n---");
        // Serial.printf("TimerAddr: %p\n", &stpTimer);
        // Serial.printf("v:%6d    v_sqr %6.0f  v_tgt: %6d\n", v, (float)v_sqr, v_tgt);
        // Serial.printf("a: %6d   twoA: %6d\n", a, twoA);
        // Serial.printf("moving: %s\n", isMoving ? "yes" : "no");

        if (!isMoving)
        {
            stpTimer = TimerFactory::makeTimer();
            stpTimer->setPulseParams(8, stepPin);
            stpTimer->attachCallbacks([this] { rotISR(); }, [this] { resetISR(); });
            v_sqr = vDir * 200 * 200;
            mode  = mode_t::rotate;
            stpTimer->start();
            isMoving = true;
        }
    }

    errorCode StepperBase::doMoveAsync(int32_t _s_tgt, uint32_t v_s, uint32_t v_e, uint32_t v_tgt, uint32_t a)
    {
        twoA  = 2 * a;
        s_tgt = _s_tgt;
        v_sqr = 0;
        v_tgt_sqr = v_tgt * v_tgt - twoA;
        //     s          = 0;
        //     int32_t ds = std::abs(_s_tgt - pos);
        //     s_tgt      = ds;

        //     dir = signum(_s_tgt - pos);
        //     digitalWriteFast(dirPin, dir > 0 ? HIGH : LOW);
        //     delayMicroseconds(5);

        //     twoA = 2 * a;
        //     //v_sqr      = (int64_t) v * v;
        //     //v_sqr = v * v;
        //     //v         = 0;
        //     v_tgt_sqr = (int64_t)v_tgt * v_tgt;

        //     int64_t accLength = std::abs(v_tgt_sqr - v_sqr) / twoA + 1;

        //     if (v_tgt_sqr >= v_sqr)
        //     {
        //          if (accLength >= s_tgt / 2) accLength = s_tgt / 2;

        //         accEnd   = accLength - 1;
        //         decStart = s_tgt - accLength;
        //     }
        //     else
        //     {
        //         twoA     = -2 * a;
        //         accEnd   = accLength - 1;
        //         decStart = s_tgt - accLength;
        //     }

        //     SerialUSB1.printf("Stepper: %s\n", name.c_str());
        //     SerialUSB1.printf("Timer: %p\n", stpTimer);
        //     SerialUSB1.printf("a: %6d   twoA:  %6d\n", a, twoA);
        //     SerialUSB1.printf("v:%6d   v_tgt: %6d\n", v, v_tgt);
        //     SerialUSB1.printf("s: %6d   s_tgt: %6d pos:%6d\n", s, s_tgt, pos);
        //     SerialUSB1.printf("aE:%6d   dS:    %6d \n\n", accEnd, decStart);

        if (stpTimer == nullptr)
        {
            if (!TimerFactory::hasFreeTimer()) return errorCode::noFreeTimer;

            stpTimer = TimerFactory::makeTimer();
            stpTimer->attachCallbacks([this] { stepISR(); }, [this] { resetISR(); });
            stpTimer->setPulseParams(8, stepPin);
            isMoving = true;
            v_sqr    = 200*200 - twoA;
     //       v_s* v_s;
            mode     = mode_t::target;
            stpTimer->start();
           // stepISR();
        }
        return errorCode::OK;
    }

    // // void StepperBase::rotateAsync()
    // // {
    // //     rotateAsync(vMax);
    // // }

    void StepperBase::doStopAsync(int32_t v_end, uint32_t a)
    {
        //if (!isMoving) return;
        mode = mode_t::stopping;
        doRotateAsync(v_end, a);
        mode = mode_t::stopping;
        // SerialUSB1.println("stoprot");
        //SerialUSB1.flush();
    }

}