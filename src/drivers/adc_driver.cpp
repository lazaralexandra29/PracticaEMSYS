#include "drivers/adc_driver.hpp"
#include <avr/io.h>
#include <util/delay.h>

constexpr uint16_t ADC_TIMEOUT = 10000;

AdcStatus AdcDriver::Init(AdcReference reference, AdcPrescaler prescaler)
{
    switch (reference)
    {
        case AdcReference::AREF:
            ADMUX &= ~(1 << REFS1);
            ADMUX &= ~(1 << REFS0);
            break;
        case AdcReference::AVCC:
            ADMUX &= ~(1 << REFS1);
            ADMUX |=  (1 << REFS0);
            break;
        case AdcReference::INTERNAL_1V1:
            ADMUX |=  (1 << REFS1);
            ADMUX &= ~(1 << REFS0);
            break;
        case AdcReference::INTERNAL_2V56:
            ADMUX |=  (1 << REFS1);
            ADMUX |=  (1 << REFS0);
            break;
        default:
            return AdcStatus(AdcErrorCode::INVALID_REFERENCE);
    }

    if (static_cast<uint8_t>(prescaler) < 1 || static_cast<uint8_t>(prescaler) > 7)
    {
        return AdcStatus(AdcErrorCode::INVALID_PRESCALER);
    }

    ADCSRA = (ADCSRA & 0xF8) | static_cast<uint8_t>(prescaler);

    ADCSRA |= (1 << ADEN);

    return AdcStatus(AdcErrorCode::SUCCESS);
}

AdcStatus AdcDriver::ReadChannel(uint8_t channel)
{
    if (channel > 7)
    {
        return AdcStatus(AdcErrorCode::INVALID_CHANNEL);
    }

    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
 
    ADCSRA |= (1 << ADSC);

    uint16_t timeout = ADC_TIMEOUT;
    while ((ADCSRA & (1 << ADSC)) && --timeout);

    if (timeout == 0)
    {
        return AdcStatus(AdcErrorCode::CONVERSION_TIMEOUT);
    }

    uint16_t adcValue = ADC;
    return AdcStatus(AdcErrorCode::SUCCESS, adcValue);
}