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
        static ITimer* getChannel(int chNr);
        static void releaseChannel(ITimer* ch);

     protected:
        static void ISR();

        static IMXRT_TMR_t* const regs;
        static bool initialized;
        static TmrChannel* channels[4];

        static constexpr IRQ_NUMBER_t IRQ = moduleNr == 0   ? IRQ_QTIMER1
                                            : moduleNr == 1 ? IRQ_QTIMER2
                                            : moduleNr == 2 ? IRQ_QTIMER3
                                                            : IRQ_QTIMER4;

        static_assert(moduleNr < 4, "Wrong TMR module number");
    };

    template <unsigned moduleNr>
    ITimer* TMRModule<moduleNr>::getChannel(int chNr)
    {
        if (!initialized)
        {
            LOG("init TMR%d", moduleNr);

            attachInterruptVector(IRQ, ISR);
            NVIC_ENABLE_IRQ(IRQ);
        }

        channels[chNr] = new TmrChannel(moduleNr, chNr);

        return channels[chNr];
    }

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    void TMRModule<moduleNr>::releaseChannel(ITimer* ch)
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
        LOG("isr");
        for (int ch = 0; ch < 4; ch++)
        {
            if (regs->CH[ch].CSCTRL & TMR_CSCTRL_TCF1)
            {
                regs->CH[ch].CSCTRL &= ~TMR_CSCTRL_TCF1;
                if (channels[ch] != nullptr)
                {
                    LOG("ch isr");
                }
            }
        }
        // {
        //     channels[ch]->regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        //     channels[ch]->ISR();
        // }

        //asm volatile("dsb"); //wait until register changes propagated through the cache
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