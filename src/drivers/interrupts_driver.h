#ifndef INTERRUPTS_DRIVER_H
#define INTERRUPTS_DRIVER_H

#include <stdint.h>
#include <avr/interrupt.h>

typedef void (*InterruptCallback_t)(void);

class InterruptsDriver {
    public: 
    static InterruptCallback_t timer0_compA_callback;
    
    int8_t AttachTimer0CompareA(InterruptCallback_t callback);

    void EnabelGlobalInterrupts(){
        sei();
    }

};

#endif