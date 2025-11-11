#include "drivers/timer_driver.hpp"
#include "app/led_manager.hpp"
#include <avr/io.h>
#include <stdint.h>

#define LED_GREEN_PIN 3    
#define LED_GREEN_PORT &PORTD


TimerDriver timerDriver;

LedManager whiteLed(LED_GREEN_PORT, LED_GREEN_PIN);

uint8_t blink_timer_id;

void BlinkWhiteCallback()
{
    whiteLed.toggle();
}

int main()
{
    whiteLed.init();

    TimerConfiguration timerConfig(TimerMode::CTC, Prescaler::DIV_64);
    if (!timerDriver.InitTimer1(timerConfig).IsSuccess())
    {
        while(1) {} 
    }

    blink_timer_id = timerDriver.CreateTimerSoftware();
    if (blink_timer_id < MAX_SOFTWARE_TIMERS)
    {
        timerDriver.RegisterPeriodicCallback(blink_timer_id, BlinkWhiteCallback, 100);
    }

    
    while (1)
    {
        timerDriver.Run();    
    }

    return 0;
}
