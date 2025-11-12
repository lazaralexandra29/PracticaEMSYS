#ifndef BUTTON_MANAGER_HPP
#define BUTTON_MANAGER_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>
#include <stdbool.h>

class ButtonManager 
{
    public:
     ButtonManager(volatile uint8_t* port, uint8_t pin);
     void init();
     bool update();
     bool isPressed();
     uint16_t getPressCount();
     static void handle();  
     static void reportState();

    private:
     GpioDriver gpio;
     PinDescription buttonPin;
     uint16_t pressCount;
     bool lastState;
     bool currentState;
};

#endif 