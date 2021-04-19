#pragma once

#include "stepperbase.h"

namespace TS4
{
    class Stepper : public StepperBase
    {
     public:
        Stepper(int stepPin, int dirPin)
            : StepperBase(stepPin, dirPin)
        {}

        int32_t getPosition() const { return pos; }
        void setPosition(int32_t p) { pos = p; }

        Stepper& setMaxSpeed(int32_t speed);          // steps/s
                                                       // StepperBase& setVStart(int32_t vIn);              // steps/s
                                                       // StepperBase& setVStop(int32_t vIn);               // steps/s
        Stepper& setAcceleration(uint32_t _a);         // steps/s^2
                                                       //
        void setTargetAbs(int32_t pos) { target = pos; }; // Set target position absolute
                                                       // void setTargetRel(int32_t delta);                 // Set target position relative to current position

        void moveAsync();
        void moveAbsAsync(int32_t target, uint32_t v = 0);
        void moveAbs(int32_t target, uint32_t v = 0);

        void moveRelAsync(int32_t delta, uint32_t v = 0);
        void moveRel(int32_t delta, uint32_t v = 0);

        void rotateAsync(int32_t v = 0);
        void stopAsync();
        void stop();



        int32_t vMax = vMaxDefault;
        uint32_t acc  = aDefault;
       // uint32_t s_t  = 0;
     protected:

        static constexpr int32_t vMaxMax       = 100'000; // largest speed possible (steps/s)
        static constexpr uint32_t aMax          = 999'999; // speed up to 500kHz within 1 s (steps/s^2)
        static constexpr uint32_t vMaxDefault   = 1'000;   // should work with every motor (1 rev/sec in 1/4-step mode)
        static constexpr uint32_t vStartDefault = 100;     // start speed
        static constexpr uint32_t vStopDefault  = 100;     // stop speed
        static constexpr uint32_t aDefault      = 1'000;   // reasonably low (~1s for reaching the default speed)

        friend class StepperGroup;
        // compare functions
        //     static bool cmpDelta(const StepperBase* a, const StepperBase* b) { return a->A > b->A; }
        //     static bool cmpAcc(const StepperBase* a, const StepperBase* b) { return a->a < b->a; }
        //     static bool cmpVmin(const StepperBase* a, const StepperBase* b) { return std::abs(a->vMax) < std::abs(b->vMax); }
        //    static bool cmpVmax(const StepperBase* a, const StepperBase* b) { return std::abs(a->vMax) > std::abs(b->vMax); }
    };
}