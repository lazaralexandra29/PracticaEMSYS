#include "config/pin_config.hpp"
#include "drivers/usart_driver.hpp"
#include "drivers/timer_driver.hpp"
#include "app/led_manager.hpp"
#include <app/adc_manager.hpp>
#include "app/button_manager.hpp"
#include <app/command_manager.hpp>
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

TimerDriver timerDriver;

LedManager greenLed(LED_GREEN_PORT, LED_GREEN_PIN);
LedManager redLed(LED_RED_PORT, LED_RED_PIN);
LedManager yellowLed(LED_YELLOW_PORT, LED_YELLOW_PIN);

ButtonManager buttonManager(BUTTON_PORT, BUTTON_PIN);

uint8_t blink_timer_id;

void BlinkWhiteCallback()
{
    greenLed.toggle();
}

int main()
{
    UsartDriver::Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);

    greenLed.init();
    redLed.init();
    yellowLed.init();

    redLed.registerInstance(1);   
    yellowLed.registerInstance(2);

    buttonManager.init();

    UsartDriver::send("System initialized. Waiting for button press...\r\n");
    
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

    char commandBuffer[64];
    uint16_t last_press_count = 0;

    while (1)
    {
        buttonManager.update();

        if (UsartDriver::receiveLineNonBlocking(commandBuffer, sizeof(commandBuffer)))
        {
            CommandManager::executeCommand(commandBuffer);
        }
        
        //uint16_t current_press_count = buttonManager.getPressCount();

        //if (current_press_count != last_press_count)
        //{
           // ButtonManager::handle(); 
           // last_press_count = current_press_count;
        //}

        timerDriver.Run();    
    }

    return 0;
}
