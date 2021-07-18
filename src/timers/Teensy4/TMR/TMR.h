#pragma once

#include "../../interfaces.h"
#include "arduino.h"
#include "imxrt.h"

namespace TS4
{
    /**
     * Teensy 4.x TMR timer
     * Implements the ITimer interface and models
     * one of the four channels of a TMR module
     **/
    class TmrTimer : public ITimer
    {
     public:
        inline TmrTimer(IMXRT_TMR_CH_t* const regs);
        ~TmrTimer() { stop(); }

        inline void setPulseParams(float width, unsigned pin);

        inline void updateFrequency(float f) override;
        inline void start() override;
        inline void stop() override;

        inline void attachCallbacks(callback_t stepCb, callback_t resetCb) override;

     protected:
        static constexpr int prescale = 5; // 1->2, 2->4, 3->8...7->128

        callback_t stepCB;
        callback_t resetCB;
        uint8_t stpPin;
        uint16_t pulsewidth;
        uint16_t period;

        IMXRT_TMR_CH_t* const regs;
        inline void ISR();

        template <unsigned>
        friend class TMRModule;

        bool first = true;
    };

    // inline implementation ===========================================================

    TmrTimer::TmrTimer(IMXRT_TMR_CH_t* const _regs)
        : regs(_regs)
    {
        period     = 1000;
        pulsewidth = 50;

        regs->CTRL   = 0x0000;
        regs->CNTR   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = pulsewidth;
        regs->CMPLD1 = pulsewidth;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF2EN;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF2;
        regs->CSCTRL = 0;
        regs->SCTRL  = 0;

        regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1000 | prescale) | TMR_CTRL_LENGTH;
    }

    void TmrTimer::start()
    {
        regs->CTRL   = 0x0000;
        regs->CNTR   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = pulsewidth;
        regs->CMPLD1 = pulsewidth;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF2EN;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF2;
        regs->CSCTRL = 0;
        regs->SCTRL  = 0;
        regs->CTRL   = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1000 | prescale) | TMR_CTRL_LENGTH;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;
        first = true;
        ISR();
    }

    void TmrTimer::stop()
    {
        // regs->SCTRL &= ~TMR_SCTRL_TCF;
        // regs->SCTRL &= ~TMR_SCTRL_TCFIE;
        //Serial.printf("stop \n");

        regs->CTRL = 0;

        // regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN;
        // regs->CSCTRL |= TMR_CSCTRL_TCF1;
    }

    void TmrTimer::updateFrequency(float f)
    {
        constexpr unsigned prescale = 32;
        period                      = (150E6 / prescale) / f - pulsewidth - 1.5f;

        //constexpr uint16_t pp = p;
    }

    void TmrTimer::attachCallbacks(callback_t stepCB, callback_t resetCB)
    {
        this->stepCB  = stepCB;
        this->resetCB = resetCB;
    }

    void TmrTimer::setPulseParams(float width_us, unsigned stpPin)
    {
        constexpr unsigned prescale = 32;
        this->pulsewidth            = width_us * (150.0f / prescale) + 0.5;
        this->stpPin                = stpPin;

        //Serial.printf("setPulseParams %d\n", pulsewidth);
    }

    void TmrTimer::ISR()
    {
        //Serial.printf("isr %p\n", regs);

        // if (regs->CSCTRL & TMR_CSCTRL_TCF1)
        // {
        //     regs->CSCTRL &= ~TMR_CSCTRL_TCF1; // clear interrupt flag
        if (first)                     // generate rising edge of pulse
        {                              //
            regs->COMP1  = pulsewidth; // set reload to pulse width
            regs->CMPLD1 = pulsewidth;
            first        = false;  // generate falling pulse edge when called next
            stepCB();              //
        }                          //
        else                       //
        {                          //
            regs->COMP1  = period; // set reload, period is already reduced by the pulsewidth time
            regs->CMPLD1 = period; //
            resetCB();             // reset the step pin
            first = true;          // generate rising edge when called next
        }
        //}
    }

    //====================================================================

    /**
     * Teensy 4.x TMR Module
     * The class implements the ITimerModule interface and
     *  models one of four TMR modules of the IMXRT1062.
     *
     * getChannel() returns a pointer to a free timer channel.
     * returnChannel(IStepTimer*) releases the passed in timer.
     **/

    template <unsigned moduleNr>
    class TMRModule : public ITimerModule
    {
     public:
        TMRModule();
        ~TMRModule();

        ITimer* getChannel();
        void releaseChannel(ITimer* ch);

     protected:
        static void ISR();

        static TmrTimer* channels[4];

        static_assert(moduleNr < 4, "Wrong TMR module number");
        static constexpr uintptr_t tmrAddresses[]{IMXRT_TMR1_ADDRESS, IMXRT_TMR2_ADDRESS, IMXRT_TMR3_ADDRESS, IMXRT_TMR4_ADDRESS};
        static bool isFree[4];
        static constexpr IRQ_NUMBER_t tmrIRQs[]{IRQ_QTIMER1, IRQ_QTIMER2, IRQ_QTIMER3, IRQ_QTIMER4};
        static IMXRT_TMR_t* const regs;
    };

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    TMRModule<moduleNr>::TMRModule()
    {
        //Serial.printf("TMRModule cstr %p %d\n", this, moduleNr);

        attachInterruptVector(tmrIRQs[moduleNr], ISR);
        NVIC_ENABLE_IRQ(tmrIRQs[moduleNr]);
    };

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    TMRModule<moduleNr>::~TMRModule()
    {
        NVIC_DISABLE_IRQ(tmrIRQs[moduleNr]);
        for (ITimer* channel : channels)
        {
            delete (channel);
        }
    }

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    ITimer* TMRModule<moduleNr>::getChannel()
    {
        //Serial.println("TMRModule::getTimer");
        //Serial.flush();

        //  isFree[0] = false;
        //  return (ITimer*)channels[0];

        for (int i = 0; i < 4; i++)
        {
            if (isFree[i])
            {
                isFree[i] = false;
                return (ITimer*)channels[i];
            }
        }

        return nullptr;
    }

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    void TMRModule<moduleNr>::releaseChannel(ITimer* ch)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ch == channels[i])
            {
                isFree[i] = true;
            }
        }
    }

    //---------------------------------------------------------------------------
    template <unsigned moduleNr>
    void TMRModule<moduleNr>::ISR()
    {
        for (int ch = 0; ch < 4; ch++)
        {
            if (!isFree[ch] && (channels[ch]->regs->CSCTRL & TMR_CSCTRL_TCF1))
            {
                channels[ch]->regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
                channels[ch]->ISR();
            }
        }
        asm volatile("dsb"); //wait until register changes propagated through the cache
    }

    // initialize static members ---------------------------------------------------------------------------------------------

    template <unsigned modNr>
    IMXRT_TMR_t* const TMRModule<modNr>::regs = ((IMXRT_TMR_t*)(tmrAddresses[modNr])); // pointer to the TMRn register block

    template <unsigned modNr>
    bool TMRModule<modNr>::isFree[4]{true, true, true, true}; // housekeeping of free channels

    template <unsigned modNr>
    TmrTimer* TMRModule<modNr>::channels[4]{
        new TmrTimer(&regs->CH[0]),
        new TmrTimer(&regs->CH[1]),
        new TmrTimer(&regs->CH[2]),
        new TmrTimer(&regs->CH[3]),
    };

}