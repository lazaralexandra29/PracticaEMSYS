#ifndef BUZZER_MANAGER_HPP
#define BUZZER_MANAGER_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>

class BuzzerManager
{
public:
    BuzzerManager(
        volatile uint8_t* leftBuzzerPort, uint8_t leftBuzzerPin,
        volatile uint8_t* rightBuzzerPort, uint8_t rightBuzzerPin
    );

    void init();
    void setState(bool active);  
    void setLeftState(bool active);
    void setRightState(bool active);
    void toggle();  
    void update(); 
    
    bool isActive() const 
    { 
        return activeState; 
    }

private:
    GpioDriver gpio;
    PinDescription leftBuzzerPin;
    PinDescription rightBuzzerPin;
    
    bool activeState;
    bool currentBuzzerState; 
};

#endif

