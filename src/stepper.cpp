#include "stepper.h"
#include "Arduino.h"
#include <algorithm>

namespace TS4
{
    Stepper& Stepper::setMaxSpeed(int32_t speed)
    {
        vMax = constrain(speed, -vMaxMax, vMaxMax);
        return *this;
    }

    Stepper& Stepper::setAcceleration(uint32_t a)
    {
        acc = std::min(a, aMax);
        return *this;
    }

    void Stepper::rotateAsync(int32_t v)
    {
        StepperBase::startRotate(v == 0 ? vMax : v, acc);
    }

    void Stepper::moveAbsAsync(int32_t target, uint32_t v)
    {
        StepperBase::startMoveTo(target, 0, (v == 0 ? vMax : v), acc);
    }

    void Stepper::moveRelAsync(int32_t delta, uint32_t v)
    {
        StepperBase::startMoveTo(pos + delta, 0, (v == 0 ? std::abs(vMax) : v), acc);
    }

    void Stepper::stopAsync()
    {
        StepperBase::startStopping(0, acc);
    }

    void Stepper::moveAsync()
    {
        StepperBase::startMoveTo(target, 0, std::abs(vMax), acc);
    }

    void Stepper::moveAbs(int32_t target, uint32_t v)
    {
        moveAbsAsync(target, v);
        while (isMoving)
        {
            delay(10);
        }
    }

    void Stepper::moveRel(int32_t delta, uint32_t v)
    {
        moveRelAsync(delta, v);
        while (isMoving)
        {
            delay(10);
        }
    }

    void Stepper::stop()
    {
        StepperBase::startStopping(0, acc);
        while (isMoving)
        {
            delay(10);
        }
    }

    // void moveRelAsync(int delta);
    // void moveRel(int delta);
}