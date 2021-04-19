#pragma once



#include "stepper.h"
#include "steppergroup.h"
#include "timers/interfaces.h"
#include "ErrorHandling/error_handler.h"

//#define TS4_NO_HIGHLEVEL_NAMESPACE

#if !defined TS4_NO_HIGHLEVEL_NAMESPACE
using Stepper = TS4::Stepper;
using StepperGroup = TS4::StepperGroup;
#endif

namespace TS4
{
    extern void begin(bool useDefaultModule = true);
}
