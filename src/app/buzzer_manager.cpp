#include "app/buzzer_manager.hpp"

BuzzerManager::BuzzerManager(
    volatile uint8_t* leftBuzzerPort, uint8_t leftBuzzerPin,
    volatile uint8_t* rightBuzzerPort, uint8_t rightBuzzerPin
) : leftBuzzerPin(leftBuzzerPort, leftBuzzerPin),
    rightBuzzerPin(rightBuzzerPort, rightBuzzerPin),
    activeState(false),
    currentBuzzerState(false)
{
}

void BuzzerManager::init()
{
    gpio.SetPinDirection(leftBuzzerPin, PinDirection::OUT);
    gpio.SetPinDirection(rightBuzzerPin, PinDirection::OUT);
    
    setState(false);
}

void BuzzerManager::setState(bool active)
{
    activeState = active;
    
    if (!active)
    {
        gpio.SetPinValue(leftBuzzerPin, PinValue::LOW);
        gpio.SetPinValue(rightBuzzerPin, PinValue::LOW);
        currentBuzzerState = false;
    }
}

void BuzzerManager::toggle()
{
    if (activeState)
    {
        currentBuzzerState = !currentBuzzerState;
        gpio.SetPinValue(leftBuzzerPin, currentBuzzerState ? PinValue::HIGH : PinValue::LOW);
        gpio.SetPinValue(rightBuzzerPin, currentBuzzerState ? PinValue::HIGH : PinValue::LOW);
    }
}

void BuzzerManager::update()
{
    if (activeState)
    {
        toggle();
    }
}

void BuzzerManager::setLeftState(bool active)
{
    if (active)
    {
        gpio.SetPinValue(leftBuzzerPin, PinValue::HIGH);
    }
    else
    {
        gpio.SetPinValue(leftBuzzerPin, PinValue::LOW);
    }
}

void BuzzerManager::setRightState(bool active)
{
    if (active)
    {
        gpio.SetPinValue(rightBuzzerPin, PinValue::HIGH);
    }
    else
    {
        gpio.SetPinValue(rightBuzzerPin, PinValue::LOW);
    }
}