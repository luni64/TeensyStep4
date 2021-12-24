#include "timerfactory.h"
#include "../ErrorHandling/error_handler.h"
#include "Teensy4/TMR/TMRChannel.h"
#include "Teensy4/TMR/TMRModule.h"
#include <memory>

// struct pwm_pin_info_struct
// {
//     uint8_t type;         // 0=no pwm, 1=flexpwm, 2=quad
//     const uint8_t module; // 0-3, 0-3
//     uint8_t channelNr;    // 0=X, 1=A, 2=B
//     uint8_t muxval;       //
// };
// extern const struct pwm_pin_info_struct pwm_pin_info[NUM_DIGITAL_PINS];

extern const struct pwm_pin_info_struct
{
    uint8_t type;         // 0=no pwm, 1=flexpwm, 2=quad
    const uint8_t module; // 0-3, 0-3
    uint8_t channelNr;    // 0=X, 1=A, 2=B
    uint8_t muxval;       //
} pwm_pin_info[NUM_DIGITAL_PINS];


namespace TS4
{
    namespace // private
    {
        enum timerType { none    = 0,
                         flexPWM = 1,
                         TMR     = 2 };
    }

    namespace TimerFactory
    {
        ITimer* makeTimer(unsigned pin)
        {
            const auto& pinInfo = pwm_pin_info[pin];
            uint8_t moduleNr    = pinInfo.module >> 4;
            uint8_t chNr        = pinInfo.module & 0b00001111;

            switch (pinInfo.type)
            {
                case timerType::TMR: {
                    switch (moduleNr)
                    {
                        case 0: return TMRModule<0>::getChannel(chNr);
                        case 1: return TMRModule<1>::getChannel(chNr);
                        case 2: return TMRModule<2>::getChannel(chNr);
                        case 3: return TMRModule<3>::getChannel(chNr);
                    }
                }
                case timerType::flexPWM:
                    break;

                default:
                    break;
            }
            return nullptr;
        }

        void returnTimer(ITimer* timer)
        {
        }
    }
}