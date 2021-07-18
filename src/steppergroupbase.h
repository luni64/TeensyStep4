#pragma once

#include "stepper.h"
#include <vector>
#include "Arduino.h"

namespace TS4
{
    class StepperGroupBase
    {
     public:
        void startMove()
        {
            if (steppers.empty()) return;


            auto deltaSorter = [](Stepper* a, Stepper* b) { return std::abs(a->target - a->pos) > std::abs(b->target - b->pos); };

            std::vector<Stepper*> sorted = steppers;              // copy stepper list..
            std::sort(sorted.begin(), sorted.end(), deltaSorter); // ...and sort by "steps to do"

            leadStepper = sorted[0]; // this stepper will lead the movement, steps of the other motors are calculated by Bresenham algorithm

            leadStepper->A       = std::abs(leadStepper->target - leadStepper->pos);
            //  SerialUSB1.printf("%s tgt:%d A:%d B:%d\n", leadStepper->name.c_str(), leadStepper->target, leadStepper->A, leadStepper->B);
            // //

            for (unsigned i = 1; i < sorted.size(); i++) // loop through the dependent motors
            {
                Stepper* stepper    = sorted[i];                       //
                int32_t delta       = stepper->target - stepper->pos;  //
                sorted[i - 1]->next = stepper;                         // set up linked list
                stepper->A          = std::abs(delta);                 //
                stepper->B          = 2 * stepper->A - leadStepper->A; // set bresenham params for dependent steppers
                stepper->dir        = (delta >= 0) ? 1 : -1;
                digitalWriteFast(stepper->dirPin, delta >= 0 ? HIGH : LOW);
                // SerialUSB1.printf("%s tgt:%d A:%d B:%d\n", stepper->name.c_str(), stepper->target, stepper->A, stepper->B);
                // SerialUSB1.flush();
            }                                    //
            sorted[sorted.size() - 1]->next = nullptr; // end of linked list
            sorted[0]->moveAsync();              // start lead stepper
        }

        void startRotate()
        {
            if (steppers.empty()) return;

            //SerialUSB1.println("srot");

            auto deltaSorter = [](Stepper* a, Stepper* b) { return a->vMax > b->vMax; };

            // SerialUSB1.printf("0: %s %d, 1: %s %d\n", steppers[0]->name.c_str(), steppers[0]->vMax, steppers[1]->name.c_str(), steppers[1]->vMax);
            // SerialUSB1.flush();

            std::vector<Stepper*> sorted = steppers;              // copy stepper list..
            std::sort(sorted.begin(), sorted.end(), deltaSorter); // ...and sort by "steps to do"

            leadStepper = sorted[0]; // this stepper will lead the movement, steps of the other motors are calculated by Bresenham algorithm
            leadStepper->A       = std::abs(leadStepper->vMax);

            //
            for (unsigned i = 1; i < sorted.size(); i++) // loop through the dependent motors
            {
                Stepper* stepper    = sorted[i];                          //
                sorted[i - 1]->next = stepper;                            // set up linked list
                stepper->A          = std::abs(stepper->vMax); //
                stepper->B          = 2 * stepper->A - leadStepper->A;    // set bresenham params for dependent steppers
                stepper->dir        = (stepper->vMax >= 0) ? 1 : -1;
                digitalWriteFast(stepper->dirPin, stepper->dir >= 0 ? HIGH : LOW);
                //Serial.printf("r %s vMax:%d A:%d B:%d\n", stepper->name.c_str(), stepper->vMax, stepper->A, stepper->B);
            }                                    //
            sorted[sorted.size() - 1]->next = nullptr; // end of linked list
            leadStepper->rotateAsync();              // start lead stepper
        }

        void stopAsync()
        {
            leadStepper->stopAsync();
        }

     protected:
        std::vector<Stepper*> steppers;

        Stepper* leadStepper;
    };
}