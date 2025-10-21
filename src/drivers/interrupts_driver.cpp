#include <drivers/interrupts_driver.h>
#include <avr/io.h>

InterruptCallback_t InterruptsDriver::timer0_compA_callback = nullptr;

int8_t InterruptsDriver::AttachTimer0CompareA(InterruptCallback_t callback){
    if(InterruptsDriver::timer0_compA_callback != nullptr){
        return -1;
    }
    InterruptsDriver::timer0_compA_callback = callback;

    TIMSK0 |= (1 << OCIE0A);

    return 0;
}

ISR(TIMER0_COMPA_vect){
    if(InterruptsDriver::timer0_compA_callback != nullptr) {
        InterruptsDriver::timer0_compA_callback();
    }
}