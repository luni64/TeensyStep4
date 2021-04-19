#include "error_handler.h"
#include "core_pins.h"

namespace TS4
{
    ErrorHandler::ErrorHandler(Stream& s) : stream(s)
    {
        s.println("TS4 error handler started");
    }

    void ErrorHandler::operator()(errorCode code, const char* msg = nullptr) const
    {
        const char* txt;

        switch (code)
        {
            case errorCode::OK:
                txt = "OK";
                break;

            // warnings
            case errorCode::message:
                txt = "Message:";
                break;

            case errorCode::emptyGroup:
                txt = "The stepper group is empty";
                break;

            // general errors

            case errorCode::reload:
                txt = "Period must not be zero";
                break;
            case errorCode::noFreeChannel:
                txt = "Timer module has no free channel";
                break;
            case errorCode::noFreeModule:
                txt = "Timer pool contains no free timer";
                break;
            case errorCode::notImplemented:
                txt = "Function not implemented for this timer";
                break;
            case errorCode::notInitialized:
                txt = "Timer not initialized or available";
                break;

            default:
                txt = "Unknown error";
                break;
        }

        if ((int)code < 0) // in case of warnings we return after printing
        {
            stream.printf("W-%i-%s %s\n", -(int)code, txt, msg ? msg : "");
            return;
        }

        stream.printf("E-%i: %s\n%s\n", (int)code, txt, msg? msg : ""); // in case of errors we don't return
        pinMode(LED_BUILTIN, OUTPUT);
        while (true)
        {
            digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
            delay(50);
        }
    }

    errorFunc_t errFunc;

    errorCode postError(errorCode e, const char* msg)
    {
        if (errFunc != nullptr && e != errorCode::OK) errFunc(e, msg);
        return e;
    }

    void attachErrFunc(errorFunc_t _errFunc)
    {
        errFunc = _errFunc;
    }

}