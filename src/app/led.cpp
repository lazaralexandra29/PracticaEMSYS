#include "app/led.hpp"
#include "drivers/gpio_driver.hpp"
#include "drivers/usart_driver.hpp"
#include <string.h>
#include <stdio.h>

Led* Led::led_instances_[MAX_LEDS] = {nullptr};

Led::Led(volatile uint8_t* port, uint8_t pin)
    : gpio_(), led_pin_(port, pin), led_index_(0)
{}

void Led::Init() 
{
    gpio_.SetPinDirection(led_pin_, PinDirection::OUT);
}

void Led::RegisterInstance(uint8_t index)
{
    if (IsIndexValid(index))
    {
        led_index_ = index;
        led_instances_[index - 1] = this;
    }
    else
    {
        UsartDriver::send("ERR: LED index out of range.\r\n");
    }
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

void Led::Handle(uint8_t index, const char* state_str)
{
    if (!IsIndexValid(index) || led_instances_[index - 1] == nullptr)
    {
        UsartDriver::send("ERR: Invalid LED index or not initialized.\r\n");
        return;
    }
    
    Led* instance = led_instances_[index - 1];

    if (strcmp(state_str, "ON") == 0)
    {
        instance->SetState(true);
    }
    else if (strcmp(state_str, "OFF") == 0)
    {
        instance->SetState(false);
    }
    else if (strcmp(state_str, "TOGGLE") == 0)
    {
        instance->Toggle();
    }
    else
    {
        UsartDriver::send("ERR: Unknown LED state. Use ON, OFF, TOGGLE.\r\n");
        return;
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "LED %u state updated.\r\n", index);
    UsartDriver::send(buffer);
}

void Led::ReportLedState(uint8_t index)
{
    if (!IsIndexValid(index) || led_instances_[index - 1] == nullptr)
    {
        UsartDriver::send("ERR: Invalid LED index or not initialized.\r\n");
        return;
    }

    Led* instance = led_instances_[index - 1];
    
    bool state = instance->GetState();
    char buffer[32];
    
    if (state)
    {
        snprintf(buffer, sizeof(buffer), "LED %u is ON.\r\n", index);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "LED %u is OFF.\r\n", index);
    }
    UsartDriver::send(buffer);
}
