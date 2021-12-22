#include "PulseGenerator.h"
#include "CallbackStateHelper.h"

struct pwm_pin_info_struct
{
    uint8_t type;          // 0=no pwm, 1=flexpwm, 2=quad
    const uint8_t module;        // 0-3, 0-3
    uint8_t channelNumber; // 0=X, 1=A, 2=B
    uint8_t muxval;        //
};
extern const struct pwm_pin_info_struct pwm_pin_info[NUM_DIGITAL_PINS];

PulseGenerator::PulseGenerator(uint8_t _pin)
    : pin(_pin)
{
}

void PulseGenerator::begin()
{
    pinMode(pin, OUTPUT);       // need that since we need to switch btween
    digitalWriteFast(pin, LOW); // timer pin and GPIO to prevent unwanted highlevel after the pulse train

    // beginDMA();
    beginTMR();
}

volatile float res;

extern uint16_t doStep();
extern int curS, curV;

void PulseGenerator::isr(pG_state_t regs)
{
    digitalWriteFast(1, HIGH);
    regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
    doStep();
    uint16_t cnt = 10000.0f / sqrtf(curV);
    regs->CMPLD2 = cnt ;

    //Serial.printf("r - S:%d, V:%d  v:%.2f\n", (int)curS, (int)curV, cnt);
    //Serial.printf("r - S:%d, V:%d  v:%d\n", (int)curS, (int)curV, cnt);
    digitalWriteFast(1, LOW);
}

void PulseGenerator::beginTMR()
{
    digitalWriteFast(0, HIGH);

    auto pinInfo = pwm_pin_info[pin];


    uint8_t moduleNumber  = pinInfo.module >> 4;
    uint8_t channelNumber = pinInfo.module & 0b00001111;

    IMXRT_TMR_t* module =
        moduleNumber == 0   ? &IMXRT_TMR1
        : moduleNumber == 1 ? &IMXRT_TMR2
        : moduleNumber == 2 ? &IMXRT_TMR3
                            : &IMXRT_TMR4;

    regs = &module->CH[channelNumber];

    Serial.printf("module %d channel %d registers: %p\n", moduleNumber, channelNumber, regs);

    regs->CTRL   = 0x0000;                                                                        // disable counter
    regs->CSCTRL = TMR_CSCTRL_CL1(1) | TMR_CSCTRL_CL2(2) | TMR_CSCTRL_CL1(1) | TMR_CSCTRL_TCF1EN ; // use preload value for COMP1

    regs->LOAD                = 0;
    regs->CNTR                = 0;
    regs->COMP1               = 1;
    regs->COMP2               = 1;
    regs->CMPLD1              = 300;  // high
    regs->CMPLD2              = 45000; // low
    regs->SCTRL               = TMR_SCTRL_OEN | TMR_SCTRL_OPS; // directly output to pin


    callback[moduleNumber] = isr;
    state[moduleNumber] = regs;
    attachInterruptVector(IRQ_QTIMER1, relay[moduleNumber]);
    NVIC_ENABLE_IRQ(IRQ_QTIMER1);
    regs->CTRL                = TMR_CTRL_CM(1) | TMR_CTRL_PCS(8 | 0) | TMR_CTRL_LENGTH | TMR_CTRL_OUTMODE(0b100);

    //delayMicroseconds(1);
    *(portConfigRegister(11)) = 0x10 | 0x01;


    digitalWriteFast(0, LOW);

    // regs->DMA  = TMR_DMA_CMPLD1DE | TMR_DMA_CMPLD2DE;
}

