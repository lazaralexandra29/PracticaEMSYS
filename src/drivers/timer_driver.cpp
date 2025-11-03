#include <drivers/timer_driver.hpp>
#include <avr/io.h>

#define MAX_NR_OF_TIMERS 3

TimerStatus TimerDriver::Init(const TimerConfiguration& desc)
{
    uint8_t timer_id = desc.GetTimerId();
    TimerMode mode = desc.GetMode();
    Prescaler prescaler = desc.GetPrescaler();

    if (timer_id >= MAX_NR_OF_TIMERS)
    {
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);
    }

    if (timer_id == 0)
    {
        if (mode == TimerMode::NORMAL)
        {
            TCCR0A &= ~((1 << WGM01) | (1 << WGM00));
        }
        else if (mode == TimerMode::CTC)
        {
            TCCR0A |= (1 << WGM01);
            TCCR0A &= ~(1 << WGM00);
        }
        else
        {
            return TimerStatus(TimerErrorCode::INVALID_TIMER_MODE);
        }
    }
    else if (timer_id == 1)
    {
        if (mode == TimerMode::NORMAL)
        {
            TCCR1B &= ~(1 << WGM12);
        }
        else if (mode == TimerMode::CTC)
        {
            TCCR1B |= (1 << WGM12);
        }
        else
        {
            return TimerStatus(TimerErrorCode::INVALID_TIMER_MODE);
        }
    }
    else if (timer_id == 2)
    {
        if (mode == TimerMode::NORMAL)
        {
            TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
        }
        else if (mode == TimerMode::CTC)
        {
            TCCR2A |= (1 << WGM21);
            TCCR2A &= ~(1 << WGM20);
        }
        else
        {
            return TimerStatus(TimerErrorCode::INVALID_TIMER_MODE);
        }
    }

    uint8_t prescalerBits = 0;

    if (timer_id == 0)
    {
        if (prescaler == Prescaler::DIV_1)
        {
            prescalerBits = (1 << CS00);
        }
        else if (prescaler == Prescaler::DIV_8)
        {
            prescalerBits = (1 << CS01);
        }
        else if (prescaler == Prescaler::DIV_64)
        {
            prescalerBits = (1 << CS01) | (1 << CS00);
        }
        else if (prescaler == Prescaler::DIV_256)
        {
            prescalerBits = (1 << CS02);
        }
        else if (prescaler == Prescaler::DIV_1024)
        {
            prescalerBits = (1 << CS02) | (1 << CS00);
        }
        else
        {
            return TimerStatus(TimerErrorCode::INVALID_PRESCALER);
        }

        TCCR0B = prescalerBits;
    }
    else if (timer_id == 1)
    {
        if (prescaler == Prescaler::DIV_1)
        {
            prescalerBits = (1 << CS10);
        }
        else if (prescaler == Prescaler::DIV_8)
        {
            prescalerBits = (1 << CS11);
        }
        else if (prescaler == Prescaler::DIV_64)
        {
            prescalerBits = (1 << CS11) | (1 << CS10);
        }
        else if (prescaler == Prescaler::DIV_256)
        {
            prescalerBits = (1 << CS12);
        }
        else if (prescaler == Prescaler::DIV_1024)
        {
            prescalerBits = (1 << CS12) | (1 << CS10);
        }
        else
        {
            return TimerStatus(TimerErrorCode::INVALID_PRESCALER);
        }

        TCCR1B = (TCCR1B & 0xF8) | prescalerBits;
    }
    else if (timer_id == 2)
    {
        if (prescaler == Prescaler::DIV_1)
        {
            prescalerBits = (1 << CS20);
        }
        else if (prescaler == Prescaler::DIV_8)
        {
            prescalerBits = (1 << CS21);
        }
        else if (prescaler == Prescaler::DIV_32)
        {
            prescalerBits = (1 << CS21) | (1 << CS20);
        }
        else if (prescaler == Prescaler::DIV_64)
        {
            prescalerBits = (1 << CS22);
        }
        else if (prescaler == Prescaler::DIV_128)
        {
            prescalerBits = (1 << CS22) | (1 << CS20);
        }
        else if (prescaler == Prescaler::DIV_256)
        {
            prescalerBits = (1 << CS22) | (1 << CS21);
        }
        else if (prescaler == Prescaler::DIV_1024)
        {
            prescalerBits = (1 << CS22) | (1 << CS21) | (1 << CS20);
        }
        else
        {
            return TimerStatus(TimerErrorCode::INVALID_PRESCALER);
        }

        TCCR2B = prescalerBits;
    }

    return TimerStatus(TimerErrorCode::SUCCESS);
}

TimerStatus TimerDriver::SetCompareValue(uint8_t timer_id, uint16_t value)
{
    if (timer_id >= MAX_NR_OF_TIMERS)
    {
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);
    }

    if (timer_id == 0)
    {
        OCR0A = static_cast<uint8_t>(value);
    }
    else if (timer_id == 1)
    {
        OCR1A = value;
    }
    else if (timer_id == 2)
    {
        OCR2A = static_cast<uint8_t>(value);
    }
    else
    {
        return TimerStatus(TimerErrorCode::INVALID_COMPARE_VALUE);
    }

    return TimerStatus(TimerErrorCode::SUCCESS);
}

TimerStatus TimerDriver::CheckElapsed(uint8_t timer_id, bool& elapsed)
{
    if (timer_id >= MAX_NR_OF_TIMERS)
    {
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);
    }

    if (timer_id == 0)
    {
        elapsed = (TIFR0 & (1 << OCF0A));
        TIFR0 |= (1 << OCF0A); 
    }
    else if (timer_id == 1)
    {
        elapsed = (TIFR1 & (1 << OCF1A));
        TIFR1 |= (1 << OCF1A);
    }
    else if (timer_id == 2)
    {
        elapsed = (TIFR2 & (1 << OCF2A));
        TIFR2 |= (1 << OCF2A);
    }

    return TimerStatus(TimerErrorCode::SUCCESS);
}

TimerStatus TimerDriver::Stop(uint8_t timer_id)
{
    if (timer_id >= MAX_NR_OF_TIMERS)
    {
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);
    }

    if (timer_id == 0)
    {
        TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
    }
    else if (timer_id == 1)
    {
        TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
    }
    else if (timer_id == 2)
    {
        TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    }

    return TimerStatus(TimerErrorCode::SUCCESS);
}

TimerStatus TimerDriver::Reset(uint8_t timer_id)
{
    if (timer_id >= MAX_NR_OF_TIMERS)
    {
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);
    }
    if (timer_id == 0)
    {
        TCNT0 = 0;
    }
    else if (timer_id == 1)
    {
        TCNT1 = 0;
    }
    else if (timer_id == 2)
    {
        TCNT2 = 0;
    }

    return TimerStatus(TimerErrorCode::SUCCESS);
}