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
        StepperGroup() = default;

        StepperGroup(Stepper* arr[], size_t n)
        {
            for (size_t i = 0; i < n; i++)
            {
                steppers.push_back(arr[i]);
                arr++;
            }
        }

        StepperGroup(std::initializer_list<Stepper*> il)
        {
            for (auto s : il)
            {
                steppers.push_back(s);
            }
        }

        StepperGroup(std::initializer_list<std::reference_wrapper<Stepper>> il)
        {
            for (auto s : il)
            {
                Stepper* p = &(s.get());
                steppers.push_back(p);
            }
        }

        void add(Stepper& s)
        {
            steppers.push_back(&s);
        }

        errorCode move()
        {
            errorCode err = moveGroup();
            if (err != errorCode::OK) return postError(err);

            while (1)
            {
                delay(5);
                bool done = true;
                for (auto stepper : steppers)
                {
                    if (stepper->isMoving) done = false;
                }
                if (done) return errorCode::OK;
            }
        }

        errorCode move(std::initializer_list<int> targets)
        {
            size_t nrOfSteppers = steppers.size();
            if (targets.size() != nrOfSteppers) return postError(errorCode::argument, "Number of targets doesn't match number of steppers");

            for (size_t i = 0; i < nrOfSteppers; i++)
            {
                steppers[i]->setTargetAbs(targets.begin()[i]);
            }
            return move();
        }

        // void rotateAsync(int32_t v1, int32_t, v2)
        // {
        //     unsigned a2 = steppers[0]->acc *steppers[1]->vMax/steppers[0]->vMax;

        //     steppers[0]->rotateAsyncB(steppers[0]->vMax, steppers[0]->acc);
        //     steppers[1]->rotateAsyncB(steppers[1]->vMax, a2);
        // }
    };
} // namespace TS4