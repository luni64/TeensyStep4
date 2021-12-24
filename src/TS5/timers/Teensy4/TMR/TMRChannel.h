#pragma once

#include "../../../CallbackStateHelper.h"
#include "../../interfaces.h"
#include "arduino.h"
#include "imxrt.h"
#include "TS5/log.h"

namespace TS4
{
    class TmrChannel : public ITimer, protected CallbackStateHelper<TmrChannel, 16, IMXRT_TMR_t*>
    {
     public:
        inline TmrChannel(IMXRT_TMR_CH_t* const regs);
        inline TmrChannel(int moduleNr, int channelNr);
        ~TmrChannel() { stop(); }

        void begin(unsigned pin, float pulseWidth) override {LOG("%d", pin)}

        inline void setPulseParams(float width, unsigned pin);

        inline void updateFrequency(float f);
        inline void start() override;
        inline void stop() override;

        //inline void attachCallbacks(callback_t stepCb, callback_t resetCb);

     protected:
        static void isr(IMXRT_TMR_t* mreg)
        {
            // for(IMXRT_TMR_CH_t& reg : mreg->CH)
            // {

            // }
        }

        static constexpr int prescale = 5; // 1->2, 2->4, 3->8...7->128

        callback_t stepCB;
        callback_t resetCB;
        uint8_t stpPin;
        uint16_t pulsewidth;
        uint16_t period;

        IMXRT_TMR_t* module;
        IMXRT_TMR_CH_t* const regs;

        inline void ISR();

        template <unsigned>
        friend class TMRModule;

        bool first = true;
    };

    // inline implementation ===========================================================

    TmrChannel::TmrChannel(int moduleNr, int channelNr)
        : module(moduleNr == 0   ? &IMXRT_TMR1
                 : moduleNr == 1 ? &IMXRT_TMR2
                 : moduleNr == 2 ? &IMXRT_TMR3
                                 : &IMXRT_TMR4),
          regs(&module->CH[channelNr])
    {

        // state[moduleNr] = module;
        // callback[moduleNr] = isr();
        // attachInterruptVector(IRQ_QTIMER1 + moduleNr, relay[moduleNr]);

        LOG("module %d channel %d registers: %p\n", moduleNr, channelNr, regs);

        regs->CTRL   = 0x0000;                                                                        // disable counter
        regs->CSCTRL = TMR_CSCTRL_CL1(1) | TMR_CSCTRL_CL2(2) | TMR_CSCTRL_CL1(1) | TMR_CSCTRL_TCF1EN; // use preload value for COMP1

        regs->LOAD   = 0;
        regs->CNTR   = 0;
        regs->COMP1  = 1;
        regs->COMP2  = 1;
        regs->CMPLD1 = 300;                           // high
        regs->CMPLD2 = 45000;                         // low
        regs->SCTRL  = TMR_SCTRL_OEN | TMR_SCTRL_OPS; // directly output to pin
    }

    // TmrChannel::TmrChannel(IMXRT_TMR_CH_t* const _regs)
    //     : regs(_regs)
    // {
    //     period     = 1000;
    //     pulsewidth = 50;

    //     regs->CTRL   = 0x0000;
    //     regs->CNTR   = 0x0000;
    //     regs->LOAD   = 0x0000;
    //     regs->COMP1  = pulsewidth;
    //     regs->CMPLD1 = pulsewidth;

    //     regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN;
    //     regs->CSCTRL &= ~TMR_CSCTRL_TCF2EN;
    //     regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
    //     regs->CSCTRL &= ~TMR_CSCTRL_TCF2;
    //     regs->CSCTRL = 0;
    //     regs->SCTRL  = 0;

    //     regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1000 | prescale) | TMR_CTRL_LENGTH;
    // }

    void TmrChannel::start()
    {
        LOG("start");
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

    void TmrChannel::stop()
    {
        // regs->SCTRL &= ~TMR_SCTRL_TCF;
        // regs->SCTRL &= ~TMR_SCTRL_TCFIE;
        //Serial.printf("stop \n");

        regs->CTRL = 0;

        // regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN;
        // regs->CSCTRL |= TMR_CSCTRL_TCF1;
    }

    void TmrChannel::updateFrequency(float f)
    {
        constexpr unsigned prescale = 32;
        period                      = (150E6 / prescale) / f - pulsewidth - 1.5f;

        //constexpr uint16_t pp = p;
    }

    //void TmrChannel::attachCallbacks(callback_t stepCB, callback_t resetCB)
    // {
    //     this->stepCB  = stepCB;
    //     this->resetCB = resetCB;
    // }

    void TmrChannel::setPulseParams(float width_us, unsigned stpPin)
    {
        constexpr unsigned prescale = 32;
        this->pulsewidth            = width_us * (150.0f / prescale) + 0.5;
        this->stpPin                = stpPin;

        //Serial.printf("setPulseParams %d\n", pulsewidth);
    }

    void TmrChannel::ISR()
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

}