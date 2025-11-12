#include "app/adc_manager.hpp"
#include "drivers/adc_driver.hpp"
#include "drivers/usart_driver.hpp"
#include <stdio.h> 
#include <stdint.h>

extern AdcDriver g_adcDriver; 

static constexpr AdcReference ADC_REF = AdcReference::AVCC; 
static constexpr AdcPrescaler ADC_PRESCALER = AdcPrescaler::DIV128; 
static constexpr uint8_t ADC_DEFAULT_CHANNEL = 0; 


extern AdcManager g_adcManagerInstance; 


void AdcManager::init() 
{
    g_adcDriver.Init(ADC_REF, ADC_PRESCALER);
}

uint16_t AdcManager::readAdcValue(uint8_t channel)
{
    AdcStatus status = g_adcDriver.ReadChannel(channel);
    
    if (status.IsSuccess())
    {
        return status.GetValue();
    }

    UsartDriver::send("ERR: ADC read failed.\r\n");
    return 0;
}

void AdcManager::handle() 
{
    uint16_t raw_value = readAdcValue(ADC_DEFAULT_CHANNEL);
    
    static constexpr uint32_t VREF_MV = 5000UL;
    static constexpr uint16_t ADC_MAX = 1023; 
    
    uint32_t voltage_mv = ((uint32_t)raw_value * VREF_MV) / ADC_MAX; 
    
    char buffer[50];
    
    snprintf(buffer, sizeof(buffer), "AdcData -> %lu mV (Raw: %hu)\r\n", voltage_mv, raw_value);
    
    UsartDriver::send(buffer);
}