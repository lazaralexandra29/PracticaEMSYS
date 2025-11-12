#include "app/led_manager.hpp"
#include "drivers/gpio_driver.hpp"
#include "drivers/usart_driver.hpp"
#include <string.h>
#include <stdio.h>

LedManager* LedManager::ledInstances[MAX_LEDS] = {NULL};

LedManager::LedManager(volatile uint8_t* port, uint8_t pin)
    : gpio(), ledPin(port, pin) 
{}

void LedManager::init() 
{
    gpio.SetPinDirection(ledPin, PinDirection::OUT);
}

void LedManager::registerInstance(uint8_t index)
{
    if (isIndexValid(index))
    {
        ledIndex = index;
        ledInstances[index - 1] = this;
    }
    else
    {
        UsartDriver::send("ERR: LED index out of range.\r\n");
    }
}

void LedManager::setState(bool on)
{
    if (on)
    {
        gpio.SetPinValue(ledPin, PinValue::HIGH);
    }
    else
    {
        gpio.SetPinValue(ledPin, PinValue::LOW);
    }
}

bool LedManager::getState() 
{
    PinValue value;
    if (gpio.ReadPinValue(ledPin, value).IsSuccess())
        return value == PinValue::HIGH;
    return false;
}

void LedManager::toggle() 
{
    gpio.PinToggle(ledPin);
}

void LedManager::handle(uint8_t index, const char* stateStr)
{
    if (!isIndexValid(index) || ledInstances[index - 1] == NULL)
    {
        UsartDriver::send("ERR: Invalid LED index or not initialized.\r\n");
        return;
    }
    
    LedManager* instance = ledInstances[index - 1];

    if (strcmp(stateStr, "ON") == 0)
        instance->setState(true);
    else if (strcmp(stateStr, "OFF") == 0)
        instance->setState(false);
    else if (strcmp(stateStr, "TOGGLE") == 0)
        instance->toggle();
    else
    {
        UsartDriver::send("ERR: Unknown LED state. Use ON, OFF, TOGGLE.\r\n");
        return;
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "LED %u state updated.\r\n", index);
    UsartDriver::send(buffer);
}

void LedManager::reportLedState(uint8_t index)
{
    if (!isIndexValid(index) || ledInstances[index - 1] == NULL)
    {
        UsartDriver::send("ERR: Invalid LED index or not initialized.\r\n");
        return;
    }

    LedManager* instance = ledInstances[index - 1];
    
    bool state = instance->getState();
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