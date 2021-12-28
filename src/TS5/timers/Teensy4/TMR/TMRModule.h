#include "Arduino.h"
#include "TMRChannel.h"
#include "TS5/log.h"
#include "TS5/timers/interfaces.h"

namespace TS4
{
    template <unsigned moduleNr>
    class TMRModule
    {
     public:
        static IPinTimer* getChannel(int chNr, StepperBase*);
        static void releaseChannel(IPinTimer* ch);

     protected:
        static void ISR();

        static IMXRT_TMR_t* const regs;
        static TmrChannel* channels[4];
        static bool initialized;

        static constexpr IRQ_NUMBER_t IRQ = moduleNr == 0   ? IRQ_QTIMER1
                                            : moduleNr == 1 ? IRQ_QTIMER2
                                            : moduleNr == 2 ? IRQ_QTIMER3
                                                            : IRQ_QTIMER4;

        static_assert(moduleNr < 4, "Wrong TMR module number");
    };

    template <unsigned moduleNr>
    IPinTimer* TMRModule<moduleNr>::getChannel(int chNr, StepperBase* stepper)
    {
        if (!initialized)
        {
            LOG("init TMR%d", moduleNr);

            attachInterruptVector(IRQ, ISR);
            NVIC_ENABLE_IRQ(IRQ);
        }

        channels[chNr] = new TmrChannel(moduleNr, chNr, stepper);

        return channels[chNr];
    }

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    void TMRModule<moduleNr>::releaseChannel(IPinTimer* ch)
    {
        // for (int i = 0; i < 4; i++)
        // {
        //     if (ch == channels[i])
        //     {
        //         isFree[i] = true;
        //         freeChannels++;
        //     }
        // }
    }

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    void TMRModule<moduleNr>::ISR()
    {
        //LOG("isr MODULE");
        for (int ch = 0; ch < 4; ch++)
        {
            if (regs->CH[ch].CSCTRL & TMR_CSCTRL_TCF1)
            {
                regs->CH[ch].CSCTRL &= ~TMR_CSCTRL_TCF1;
                if (channels[ch] != nullptr)
                {
                    digitalToggleFast(LED_BUILTIN);

                    LOG("%d\n", micros());
                }
                //asm volatile("dsb"); //wait until register changes propagated through the cache
            }
        }
        // {
        //     channels[ch]->regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        //     channels[ch]->ISR();
        // }

        digitalWriteFast(14, LOW);
    }

    // initialize static members ---------------------------------------------------------------------------------------------

    template <unsigned moduleNr>
    IMXRT_TMR_t* const TMRModule<moduleNr>::regs = moduleNr == 0   ? &IMXRT_TMR1
                                                   : moduleNr == 1 ? &IMXRT_TMR2
                                                   : moduleNr == 2 ? &IMXRT_TMR3
                                                                   : &IMXRT_TMR4;

    template <unsigned moduleNr>
    bool TMRModule<moduleNr>::initialized = false;

    template <unsigned modNr>
    TmrChannel* TMRModule<modNr>::channels[4]{nullptr, nullptr, nullptr, nullptr};
}