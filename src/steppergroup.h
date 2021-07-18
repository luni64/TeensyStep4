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
        StepperGroup()
        {}

        StepperGroup(Stepper* arr[], size_t n)
        {
            for (size_t i = 0; i < n; i++)
            {
                // steppers.push_back(arr[i]);
                arr++;
            }
        }

        StepperGroup(std::initializer_list<std::reference_wrapper<Stepper>> il)
        {
            //Serial.println("ctr ref list");
            for (auto s : il)
            {
                Stepper* p = &(s.get());
                steppers.push_back(p);
            }
        }

        static StepperGroup make(std::initializer_list<std::reference_wrapper<Stepper>> il)
        {
            return StepperGroup(il);
        }

        void add(Stepper& s)
        {
            steppers.push_back(&s);
        }

        void move()
        {
            startMove();
            while (1)
            {
                delay(5);
                //SerialUSB1.print('.');
                bool done = true;
                for (auto stepper : steppers)
                {
                    if (stepper->isMoving) done = false;
                }
                if (done) return;
            }
        }

        // void rotateAsync(int32_t v1, int32_t, v2)
        // {
        //     unsigned a2 = steppers[0]->acc *steppers[1]->vMax/steppers[0]->vMax;

        //     steppers[0]->rotateAsyncB(steppers[0]->vMax, steppers[0]->acc);
        //     steppers[1]->rotateAsyncB(steppers[1]->vMax, a2);
        // }
    };
} // namespace TS4