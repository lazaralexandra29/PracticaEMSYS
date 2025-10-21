#include <drivers/timer_driver.h>
#include <avr/io.h>

static uint8_t GetPrescalerBits(uint8_t timer_id, Prescaler prescaler) {
    switch (prescaler) {
        case Prescaler::DIV_0: return 0;
    
        case Prescaler::DIV_1: return 1;

        case Prescaler::DIV_8: return 2;

        case Prescaler::DIV_32:
         if(timer_id == 2) return 3;
        return 0;
        break;

        case Prescaler::DIV_64:
         if(timer_id == 2) return 4;
        return 3;

        case Prescaler::DIV_128:
         if(timer_id == 2) return 5;
        return 0;

        case Prescaler::DIV_256:
         if(timer_id == 2) return 6;
        return 4;

        case Prescaler::DIV_1024:
         if(timer_id == 2) return 7;
        return 5;

        default:
         return 0;
    }

}

int8_t TimerDriver :: Init(uint8_t timer_id, TimerMode mode, Prescaler prescaler) {
    uint8_t cs_bits = GetPrescalerBits(timer_id, prescaler);

    switch (timer_id) {
        case 0:
         TCCR0A = (mode == TimerMode::CTC) ? (1 << WGM01) : 0;
         TCCR0B = cs_bits;
         TCNT0 = 0;
        break;

        case 1:
         TCCR1A = 0;
         TCCR1B = (mode == TimerMode::CTC) ? (1 << WGM12) : 0;
         TCCR1B |= cs_bits;
         TCNT1 = 0;
        break;

        case 2:
         TCCR2A = (mode == TimerMode::CTC) ? (1 << WGM21) : 0;
         TCCR2B = cs_bits;
         TCNT2 = 0;
        break;

        default:
         return -1;
    }
    return 0;
}

int8_t TimerDriver::SetCompareValue(uint8_t timer_id, uint16_t value) {
    if( (timer_id ==0 || timer_id == 2) && value > 255) return -2;

    switch (timer_id) {
        case 0:
         OCR0A = static_cast<uint8_t>(value);
        break;

        case 1:
         OCR1A = value;
        break;

        case 2:
         OCR2A = static_cast<uint8_t>(value);
        break;

        default:
         return -1;
    }
    return 0;
}

bool TimerDriver::HasElapsed(uint8_t timer_id) {
    bool elapsed = false;

    switch (timer_id) {
        case 0: 
         elapsed = (TIFR0 & (1 << OCF0A));
         if(elapsed) TIFR0 = (1 << OCF0A);
        break;

        case 1: 
         elapsed = (TIFR1 & (1 << OCF1A));
         if(elapsed) TIFR1 = (1 << OCF1A);
        break;

        case 2: 
         elapsed = (TIFR2 & (1 << OCF2A));
         if(elapsed) TIFR2 = (1 << OCF2A);
        break;
    }

    return elapsed;
}