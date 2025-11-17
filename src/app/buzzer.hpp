#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>

class Buzzer
{
public:
    Buzzer(volatile uint8_t* buzzer_port, uint8_t buzzer_pin);

    void Init();
    void SetState(bool active);  
    void Toggle();  
    void Update(); 
    
    bool IsActive() const 
    { 
        return active_state_; 
    }

private:
    GpioDriver gpio_;
    PinDescription buzzer_pin_;
    
    bool active_state_;
    bool current_buzzer_state_; 
};

#endif

