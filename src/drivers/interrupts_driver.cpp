#include <drivers/interrupts_driver.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Definirea (initializarea) variabilelor statice ale clasei (FARA Timer0)
InterruptCallback_t InterruptsDriver::timer1_compA_callback = nullptr;
InterruptCallback_t InterruptsDriver::ext_int0_callback = nullptr;

// ----------------------------------------------------------------------
// 1. Timer1 Compare Match A Implementation
// ----------------------------------------------------------------------
int8_t InterruptsDriver::AttachTimer1CompareA(InterruptCallback_t callback) {
    if (InterruptsDriver::timer1_compA_callback != nullptr) {
        return -1;
    }
    InterruptsDriver::timer1_compA_callback = callback;
    
    TIMSK1 |= (1 << OCIE1A);

    return 0;
}

// Rutina de serviciu a întreruperii (ISR) pentru Timer1 Compare Match A
ISR(TIMER1_COMPA_vect) {
    if (InterruptsDriver::timer1_compA_callback != nullptr) {
        InterruptsDriver::timer1_compA_callback();
    }
}

// ➡️ Eliminat: Implementarea si ISR-ul pentru AttachTimer0CompareA

// ----------------------------------------------------------------------
// 2. External Interrupt 0 Implementation (Buton)
// ----------------------------------------------------------------------
int8_t InterruptsDriver::AttachExternalInt0(InterruptCallback_t callback) {
    if (InterruptsDriver::ext_int0_callback != nullptr) {
        return -1;
    }
    InterruptsDriver::ext_int0_callback = callback;

    // Configurare registru (Declansare la Flanc de Coborare - Falling Edge)
    EICRA |= (1 << ISC01);  
    EICRA &= ~(1 << ISC00); 

    // Activeaza intreruperea externa 0 (INT0)
    EIMSK |= (1 << INT0); 

    return 0;
}

ISR(INT0_vect) {
    if (InterruptsDriver::ext_int0_callback != nullptr) {
        InterruptsDriver::ext_int0_callback();
    }
}