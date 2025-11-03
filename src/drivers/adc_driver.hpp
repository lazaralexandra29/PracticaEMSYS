#ifndef PRACTICAEMSYS_SRC_DRIVERS_ADC_DRIVER_HPP
#define PRACTICAEMSYS_SRC_DRIVERS_ADC_DRIVER_HPP

#include <stdint.h>

enum class AdcReference : uint8_t
{
    AREF = 0,
    AVCC = 1,
    INTERNAL_1V1 = 2,
    INTERNAL_2V56 = 3
};

enum class AdcPrescaler : uint8_t
{
    DIV2 = 1,
    DIV4 = 2,
    DIV8 = 3,
    DIV16 = 4,
    DIV32 = 5,
    DIV64 = 6,
    DIV128 = 7
};

enum class AdcErrorCode : int8_t
{
    SUCCESS = 0,
    INVALID_CHANNEL = -1,
    INVALID_REFERENCE = -2,
    INVALID_PRESCALER = -3,
    CONVERSION_TIMEOUT = -4
};

class AdcStatus
{
    public:
        AdcStatus(AdcErrorCode errorCode = AdcErrorCode::SUCCESS)
            : m_errorCode(errorCode) {}

        AdcErrorCode GetErrorCode() const
        {
            return m_errorCode;
        }

        uint16_t GetValue() const
        {
            return value_;
        }

        bool IsSuccess() const
        {
            return m_errorCode == AdcErrorCode::SUCCESS;
        }

    private:
        const AdcErrorCode m_errorCode;
        uint16_t value_;
};

class AdcDescription // AdcConfiguration
{
    public:
        AdcDescription(volatile uint8_t* admux, volatile uint8_t* adcsra, volatile uint8_t* adch, volatile uint8_t* adcl)
            : m_admux(admux), m_adcsra(adcsra), m_adch(adch), m_adcl(adcl) {}

        volatile uint8_t* GetADMUX() const 
        { 
            return m_admux; 
        }

        volatile uint8_t* GetADCSRA() const 
        { 
            return m_adcsra; 
        }

        volatile uint8_t* GetADCH() const 
        { 
            return m_adch; 
        }

        volatile uint8_t* GetADCL() const 
        { 
            return m_adcl; 
        }

    private:
        volatile uint8_t* m_admux;
        volatile uint8_t* m_adcsra;
        volatile uint8_t* m_adch;
        volatile uint8_t* m_adcl;
};

class AdcDriver
{
    public:
        AdcStatus Init(AdcReference reference, AdcPrescaler prescaler); // check prescaler la adc ?!

        AdcStatus ReadChannel(uint8_t channel);
};

#endif