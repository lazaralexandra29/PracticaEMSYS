#include "app/led.hpp"
#include "drivers/gpio_driver.hpp"

Led::Led(volatile uint8_t* port, uint8_t pin, ILogger* logger)
    : gpio_(), led_pin_(port, pin), logger_(logger)
{}

void Led::Init() 
{
    gpio_.SetPinDirection(led_pin_, PinDirection::OUT);
}

void Led::SetState(bool on)
{
    if (on)
    {
        gpio_.SetPinValue(led_pin_, PinValue::HIGH);
    }
    else
    {
        gpio_.SetPinValue(led_pin_, PinValue::LOW);
    }
}

bool Led::GetState() const
{
    PinValue value;
    if (gpio_.ReadPinValue(led_pin_, value).IsSuccess())
    {
        return value == PinValue::HIGH;
    }
    return false;
}

void Led::Toggle() 
{
    gpio_.PinToggle(led_pin_);
}