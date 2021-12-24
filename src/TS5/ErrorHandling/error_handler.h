#pragma once

#include "Stream.h"
#include "error_codes.h"
#include <functional>

namespace TS4
{
    using errorFunc_t = std::function<void(errorCode, const char*)>;

    class ErrorHandler
    {
     public:
        ErrorHandler(Stream& s);
        void operator()(errorCode code, const char* msg) const;

     protected:
        Stream& stream;
    };

    extern void attachErrFunc(errorFunc_t);
    extern errorCode postError(errorCode, const char* msg = nullptr);

}