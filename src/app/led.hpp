#ifndef LED_HPP
#define LED_HPP

#include "drivers/gpio_driver.hpp"
#include "interfaces/ilogger.hpp"
#include <stdint.h>
#include <stdbool.h>

class Led
{
public:
    Led(volatile uint8_t* port, uint8_t pin, ILogger* logger = nullptr);

    void Init();
    void SetState(bool on);
    bool GetState() const;
    void Toggle();

private:
    GpioDriver gpio_;
    PinDescription led_pin_;
    ILogger* logger_;
};

#endif
