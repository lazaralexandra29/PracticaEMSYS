#ifndef PRACTICAEMSYS_SRC_DRIVERS_GPIO_DRIVER_HPP
#define PRACTICAEMSYS_SRC_DRIVERS_GPIO_DRIVER_HPP


#include <stdint.h>

enum class PinDirection : uint8_t
{ 
    IN = 0,
    OUT = 1
};

enum class PinValue : uint8_t 
{
    LOW = 0,
    HIGH = 1
};

enum class GpioErrorCode : int8_t
{
    SUCCESS = 0, 
    PIN_INDEX_OUT_OF_RANGE = -1,
    INVALID_PIN_DIRECTION = -2,
    INVALID_PIN_VALUE = -3
};

class GpioStatus
{
    public:
     GpioStatus(GpioErrorCode errorCode = GpioErrorCode::SUCCESS) 
        : m_errorCode(errorCode), m_pinValue(PinValue::LOW) {}

     GpioStatus(GpioErrorCode errorCode, PinValue pinValue) 
        : m_errorCode(errorCode), m_pinValue(pinValue) {}
    
        GpioErrorCode GetErrorCode() const 
        { 
            return m_errorCode; 
        }

        PinValue GetPinValue() const 
        { 
            return m_pinValue; 
        }

        bool IsSuccess() const 
        { 
            return m_errorCode == GpioErrorCode::SUCCESS; 
        }

    private:
     const GpioErrorCode m_errorCode;
     const PinValue m_pinValue; 
};

class PinDescription
{
    public:
     PinDescription(volatile uint8_t *port, uint8_t pin) 
        : m_port(port), m_pin(pin)
        {}
        
        volatile uint8_t* GetPort() const 
        { 
            return m_port; 
        }

        uint8_t GetPin() const 
        { 
            return m_pin; 
        }

    private:
        volatile uint8_t* m_port;
        uint8_t m_pin;
};

class GpioDriver 
{
    public:
        GpioStatus SetPinDirection (const PinDescription& pinDesc, PinDirection direction);

        GpioStatus SetPinValue (const PinDescription& pinDesc, PinValue value);

        GpioStatus ReadPinValue (const PinDescription& pinDesc);
        
        GpioStatus PinToggle (const PinDescription& pinDesc);
};

#endif
