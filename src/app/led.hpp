#ifndef LED_HPP
#define LED_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEDS 3

class Led 
{
public:
    Led(volatile uint8_t* port, uint8_t pin);

    void Init();
    void SetState(bool on);
    bool GetState() const;
    void Toggle();

    void RegisterInstance(uint8_t index);
 
    static void Handle(uint8_t index, const char* state_str);
    static void ReportLedState(uint8_t index);

private:
    GpioDriver gpio_;           
    PinDescription led_pin_;  
    uint8_t led_index_; 

    static Led* led_instances_[MAX_LEDS];
    static bool IsIndexValid(uint8_t index) { return index > 0 && index <= MAX_LEDS; }
};

#endif
