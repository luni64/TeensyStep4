#include "TMR.h"

//#if defined (ARDUINO_TEENSY_MICROMOD)

namespace TS4
{
    // void TmrTimer::aquire()
    // {
    //     Serial.println("aquire");

    //     IMXRT_TMR_t* tmrRegs = &IMXRT_TMR4;
    //    // tmrRegs->ENBL        = 0xF;
    //     chRegs               = &(tmrRegs->CH[0]);
    //     chRegs->CTRL         = 0x0000;
    //     chRegs->LOAD         = 0x0000;
    //     chRegs->COMP1        = 65000;//46'869 - 1;
    //     chRegs->CMPLD1       = 65000; //46'869 - 1;
    //     chRegs->COMP2       =  -5+1;
    //     chRegs->CMPLD2       = -5+1;
    //     chRegs->CNTR         = 0x0000;

    //     chRegs->CSCTRL = TMR_CSCTRL_TCF1EN | TMR_CSCTRL_TCF2EN | TMR_CSCTRL_CL1(0b10) | TMR_CSCTRL_CL2(0b01);

    //     chRegs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1110) | TMR_CTRL_LENGTH;

    //     // chRegs->CSCTRL &= ~TMR_CSCTRL_TCF1;
    //     // chRegs->CSCTRL |= TMR_CSCTRL_TCF1EN;

    //     attachInterruptVector(IRQ_QTIMER4, isr); // start
    //     NVIC_ENABLE_IRQ(IRQ_QTIMER4);

    // callback_t TmrTimer::stepCB;
    // callback_t TmrTimer::resetCB;

   
}

//#endif