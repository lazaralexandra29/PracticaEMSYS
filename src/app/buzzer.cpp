#include "app/buzzer.hpp"

Buzzer::Buzzer(volatile uint8_t* buzzer_port, uint8_t buzzer_pin)
    : buzzer_pin_(buzzer_port, buzzer_pin),
      active_state_(false),
      current_buzzer_state_(false)
{
}

void Buzzer::Init()
{
    gpio_.SetPinDirection(buzzer_pin_, PinDirection::OUT);
    SetState(false);
}

void Buzzer::SetState(bool active)
{
    active_state_ = active;
    
    if (active)
    {
        gpio_.SetPinValue(buzzer_pin_, PinValue::HIGH);
        current_buzzer_state_ = true;
    }
    else
    {
        gpio_.SetPinValue(buzzer_pin_, PinValue::LOW);
        current_buzzer_state_ = false;
    }
}

void Buzzer::Toggle()
{
    if (active_state_)
    {
        current_buzzer_state_ = !current_buzzer_state_;
        
        if (current_buzzer_state_)
        {
            gpio_.SetPinValue(buzzer_pin_, PinValue::HIGH);
        }
        else
        {
            gpio_.SetPinValue(buzzer_pin_, PinValue::LOW);
        }
    }
}

void Buzzer::Update()
{
    if (active_state_)
    {
        Toggle();
    }
}