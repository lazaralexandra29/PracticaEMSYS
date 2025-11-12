#include "app/button_manager.hpp"
#include "drivers/gpio_driver.hpp"
#include "drivers/usart_driver.hpp"
#include <stdio.h>

extern ButtonManager buttonManager;

ButtonManager::ButtonManager(volatile uint8_t* port, uint8_t pin)
    : buttonPin(port, pin), pressCount(0), lastState(false) {}

void ButtonManager::init() 
{
    gpio.SetPinDirection(buttonPin, PinDirection::IN, true);
}

bool ButtonManager::update() 
{
    PinValue value;
    if (gpio.ReadPinValue(buttonPin, value).IsSuccess()) 
    {
        bool current = (value == PinValue::LOW);
        if (current && !lastState) 
        {
            pressCount++;
        }
        lastState = current;
        currentState = current;
        return currentState;
    }

    currentState = false;
    return false;
}

bool ButtonManager::isPressed() 
{
    return currentState;
}

uint16_t ButtonManager::getPressCount() 
{
    return pressCount;
}

void ButtonManager::handle()
{
    bool is_currently_pressed = buttonManager.update();

    if (is_currently_pressed)
    {
        UsartDriver::send("Button is PRESSED.\r\n");
    }
    else
    {
        UsartDriver::send("Button is NOT pressed.\r\n");
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Press count: %u\r\n", buttonManager.getPressCount());
    UsartDriver::send(buffer);
}

void ButtonManager::reportState()
{
    char buffer[64];
    
    if (buttonManager.isPressed())
    {
        UsartDriver::send("Button State: PRESSED\r\n");
    }
    else
    {
        UsartDriver::send("Button State: NOT PRESSED\r\n");
    }

    snprintf(buffer, sizeof(buffer), "Press Count: %u\r\n", buttonManager.getPressCount());
    UsartDriver::send(buffer);
}
