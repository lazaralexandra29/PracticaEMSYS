#include <drivers/gpio_driver.hpp>

#define MAX_NR_OF_PINS_PER_PORT 7

GpioStatus GpioDriver::SetPinDirection(const PinDescription& pinDesc, PinDirection direction, bool pullup)
{
    uint8_t pin = pinDesc.GetPin();
    volatile uint8_t* port = pinDesc.GetPort();

    volatile uint8_t* ddr = port - 1;

    if (pin > MAX_NR_OF_PINS_PER_PORT)
    {
        return GpioStatus(GpioErrorCode::PIN_INDEX_OUT_OF_RANGE);
    }

    if (direction == PinDirection::OUT)
    {
        *ddr |= (1 << pin);
    }
    else if (direction == PinDirection::IN)
    {
        *ddr &= ~(1 << pin);

        if(pullup)
        {
            *port |= (1 << pin);
        }
        else
        {
            *port &= ~(1 << pin);
        }
    }
    else
    {
        return GpioStatus(GpioErrorCode::INVALID_PIN_DIRECTION);
    }

    return GpioStatus(GpioErrorCode::SUCCESS);
}

GpioStatus GpioDriver::SetPinValue (const PinDescription& pinDesc, PinValue value)
{
    uint8_t pin = pinDesc.GetPin();
    volatile uint8_t* port = pinDesc.GetPort();

    if (pin > MAX_NR_OF_PINS_PER_PORT)
    {
        return GpioStatus(GpioErrorCode::PIN_INDEX_OUT_OF_RANGE);
    }

    if (value == PinValue::HIGH)
    {
        *port |= (1 << pin);
    }
    else if (value == PinValue::LOW)
    {
        *port &= ~(1 << pin);
    }
    else
    {
        return GpioStatus(GpioErrorCode::INVALID_PIN_VALUE);
    }
    
    return GpioStatus(GpioErrorCode::SUCCESS);
}

GpioStatus GpioDriver::ReadPinValue (const PinDescription& pinDesc,  PinValue& outValue)
{
    uint8_t pin = pinDesc.GetPin();
    volatile uint8_t* port = pinDesc.GetPort();
    volatile uint8_t* pinReg = port - 2;


    if (pin >= MAX_NR_OF_PINS_PER_PORT)
    {
        return GpioStatus(GpioErrorCode::PIN_INDEX_OUT_OF_RANGE); 
    }

    if (*pinReg  & (1 << pin))
    {
        outValue = PinValue::HIGH;
    }
    else
    {
        outValue = PinValue::LOW;
    }

    return GpioStatus(GpioErrorCode::SUCCESS);
}

GpioStatus GpioDriver::PinToggle (const PinDescription& pinDesc)
{
    uint8_t pin = pinDesc.GetPin();
    volatile uint8_t* port = pinDesc.GetPort();

    if (pin > MAX_NR_OF_PINS_PER_PORT)
    {
        return GpioStatus(GpioErrorCode::PIN_INDEX_OUT_OF_RANGE);
    }

    *port ^= (1 << pin);

    return GpioStatus(GpioErrorCode::SUCCESS);
}