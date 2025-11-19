#include "app/button.hpp"
#include "drivers/gpio_driver.hpp"

Button::Button(volatile uint8_t* port, uint8_t pin)
    : button_pin_(port, pin), press_count_(0), last_state_(false), current_state_(false) {}

void Button::Init() 
{
    gpio_.SetPinDirection(button_pin_, PinDirection::IN, true);
}

bool Button::Update() 
{
    PinValue value;
    if (gpio_.ReadPinValue(button_pin_, value).IsSuccess()) 
    {
        bool current = (value == PinValue::LOW);
        if (current && !last_state_) 
        {
            press_count_++;
        }
        last_state_ = current;
        current_state_ = current;
        return current_state_;
    }

    current_state_ = false;
    return false;
}

bool Button::IsPressed() const
{
    return current_state_;
}

uint16_t Button::GetPressCount() const
{
    return press_count_;
}

