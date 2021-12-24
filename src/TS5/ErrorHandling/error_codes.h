#pragma once

namespace TS4
{

    enum class errorCode
    {
        OK =                 0,

        // Warnings
        message=          -1,
        emptyGroup=       -100,


        //General errors
        argument =         100,
        callback=          101,
        reload=            102,
        noModule =     103,
        noFreeChannel =    104,  // requested module has no free channel
        notImplemented=    105,  // timer does not support this feature
        notInitialized=    106,


        // Timer Errors
        noFreeTimer =      200,
        GTP_err2 =         201,

        //TMR Errors
        TMR_err =          300,
        TMR_err2 =         301,

        //FTM Errors
        FTM_err =          400,
        FTM_err2 =         401,

        //TCK Errors
        TCK_err =          900,
        TCK_err2 =         901,
    };
}