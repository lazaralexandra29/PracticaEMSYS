#ifndef LED_MANAGER_HPP
#define LED_MANAGER_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEDS 3

class LedManager 
{
    public:
     LedManager(volatile uint8_t* port, uint8_t pin);

     void init();
     void setState(bool on);
     bool getState();
     void toggle();

     void registerInstance(uint8_t index);
 
     static void handle(uint8_t index, const char* stateStr);
     static void reportLedState(uint8_t index);

    private:
     GpioDriver gpio;           
     PinDescription ledPin;  
     uint8_t ledIndex;
 
     static LedManager* ledInstances[MAX_LEDS];
     static bool isIndexValid(uint8_t index) { return index > 0 && index <= MAX_LEDS; }
      

};

#endif
