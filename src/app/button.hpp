#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>
#include <stdbool.h>

class Button 
{
public:
    Button(volatile uint8_t* port, uint8_t pin);
    void Init();
    bool Update();
    bool IsPressed() const;
    uint16_t GetPressCount() const;

private:
    GpioDriver gpio_;
    PinDescription button_pin_;
    uint16_t press_count_;
    bool last_state_;
    bool current_state_;
};

#endif 