#pragma once
#include "stepper.h"
#include "steppergroupbase.h"
#include <algorithm>
#include <vector>

namespace TS4
{
    class StepperGroup : public StepperGroupBase
    {
     public:
        // construction ---------------------------------------------------------------------------------
        StepperGroup() = default;
        StepperGroup(Stepper* arr[], size_t n) { add(arr, n); }
        StepperGroup(std::initializer_list<std::reference_wrapper<Stepper>> stepperList) { add(stepperList); }

        // add and remove steppers ----------------------------------------------------------------------------
        void add(Stepper& s) { add(&s); }
        void add(Stepper* s) { steppers.push_back(s); }

        void add(std::initializer_list<std::reference_wrapper<Stepper>> stepperList)
        {
            for (auto& s : stepperList)
            {
                add(s.get());
            }
        }

        void add(Stepper* arr[], size_t n)
        {
            for (unsigned i = 0; i < n; i++)
            {
                add(arr[i]);
            }
        }

        void remove(Stepper& s) { remove(&s); }
        void remove(Stepper* s) { steppers.erase(std::remove(steppers.begin(), steppers.end(), s), steppers.end()); }

        void clear() { steppers.clear(); }



        void move()
        {
            startMove(); // start movement in the background
            while (1)    // wait until all steppers have stopped
            {
                delay(1);
                bool done = true;
                for (auto stepper : steppers)
                {
                    if (stepper->isMoving)
                        done = false;
                }
                if (done) break;
            }
        }

        // void rotateAsync(int32_t v1, int32_t, v2)
        // {
        //     unsigned a2 = steppers[0]->acc *steppers[1]->vMax/steppers[0]->vMax;

        //     steppers[0]->rotateAsyncB(steppers[0]->vMax, steppers[0]->acc);
        //     steppers[1]->rotateAsyncB(steppers[1]->vMax, a2);
        // }
    };
}