#include "drivers/timer_driver.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

#define MAX_SOFTWARE_TIMERS 8

struct SoftwareTimer
{
    bool active;
    uint32_t period_ms;
    uint32_t elapsed_ms;
    void (*callback)();
};

static SoftwareTimer timers[MAX_SOFTWARE_TIMERS];

static volatile uint16_t global_tick_counter = 0;

static volatile bool timer_tick_flag = false;

TimerStatus TimerDriver::InitTimer1(const TimerConfiguration& config)
{
    TCCR1A = 0;
    TCCR1B = 0;

    switch (config.GetMode())
    {
        case TimerMode::CTC:
            TCCR1B |= (1 << WGM12);
            break;
        case TimerMode::NORMAL:
        TCCR1B &= ~(1 << WGM12); 
            break;
        default:
            return TimerStatus(TimerErrorCode::INVALID_TIMER_MODE);
    }

    switch (config.GetPrescaler())
    {
        case Prescaler::DIV_1:
            TCCR1B |= (1 << CS10);
            break;
        case Prescaler::DIV_8:
            TCCR1B |= (1 << CS11);
            break;
        case Prescaler::DIV_64:
            TCCR1B |= (1 << CS11) | (1 << CS10);
            break;
        case Prescaler::DIV_256:
            TCCR1B |= (1 << CS12);
            break;
        case Prescaler::DIV_1024:
            TCCR1B |= (1 << CS12) | (1 << CS10);
            break;
        default:
            return TimerStatus(TimerErrorCode::INVALID_PRESCALER);
    }

    OCR1A = 249;

    TIMSK1 |= (1 << OCIE1A);

    sei();

    return TimerStatus(TimerErrorCode::SUCCESS);
}

uint8_t TimerDriver::CreateTimerSoftware()
{
    for (uint8_t i = 0; i < MAX_SOFTWARE_TIMERS; ++i)
    {
        if (!timers[i].active)
        {
            timers[i].active = true;
            timers[i].elapsed_ms = 0;
            timers[i].period_ms = 0;
            timers[i].callback = nullptr;
            return i;
        }
    }
    return MAX_SOFTWARE_TIMERS;
}


TimerStatus TimerDriver::RegisterPeriodicCallback(uint8_t timer_id, void (*callback)(), uint32_t period_ms)
{
    if (timer_id >= MAX_SOFTWARE_TIMERS || callback == nullptr || period_ms == 0)
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);

    timers[timer_id].callback = callback;
    timers[timer_id].period_ms = period_ms;
    timers[timer_id].elapsed_ms = 0;
    timers[timer_id].active = true;

    return TimerStatus(TimerErrorCode::SUCCESS);
}

TimerStatus TimerDriver::Stop(uint8_t timer_id)
{
    if (timer_id >= MAX_SOFTWARE_TIMERS)
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);

    timers[timer_id].active = false;
    return TimerStatus(TimerErrorCode::SUCCESS);
}

TimerStatus TimerDriver::UnregisterPeriodicCallback(uint8_t timer_id)
{
    if (timer_id >= MAX_SOFTWARE_TIMERS)
        return TimerStatus(TimerErrorCode::INVALID_TIMER_ID);

    timers[timer_id].callback = nullptr;
    timers[timer_id].active = false;
    return TimerStatus(TimerErrorCode::SUCCESS);
}

void TimerDriver::Run()
{
    if (timer_tick_flag)
    {
        cli();
        timer_tick_flag = false;
        sei();

        for (uint8_t i = 0; i < MAX_SOFTWARE_TIMERS; ++i)
        {
            if (timers[i].active && timers[i].callback)
            {
                timers[i].elapsed_ms++;
                if (timers[i].elapsed_ms >= timers[i].period_ms)
                {
                    timers[i].elapsed_ms = 0;
                    timers[i].callback();
                }
            }
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    timer_tick_flag = true;
}
