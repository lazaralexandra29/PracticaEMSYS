#ifndef INTERRUPTS_DRIVER_H
#define INTERRUPTS_DRIVER_H

#include <stdint.h>
#include <avr/interrupt.h>

typedef void (*InterruptCallback_t)(void);

class InterruptsDriver {
    public: 
    static InterruptCallback_t timer1_compA_callback;
    static InterruptCallback_t ext_int0_callback;     
    
   
    int8_t AttachTimer1CompareA(InterruptCallback_t callback); 
    int8_t AttachExternalInt0(InterruptCallback_t callback);  
    
    void EnabelGlobalInterrupts(){
        sei();
    }
};

#endif