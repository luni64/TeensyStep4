#pragma once

#include "stepper.h"
#include "steppergroup.h"
#include "timers/interfaces.h"

//#define TS4_NO_HIGHLEVEL_NAMESPACE

#if defined TS4_NO_HIGHLEVEL_NAMESPACE
using Stepper = TS4::Stepper;
using StepperGroup = TS4::StepperGroup;
#endif

namespace TS4
{
    //using Stepper = StepperBase;
    extern void begin(bool useDefaultModule = true);
}
