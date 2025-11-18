#include "app/adc.hpp"
#include "drivers/adc_driver.hpp"
#include "drivers/usart_driver.hpp"
#include <stdio.h> 
#include <stdint.h>

extern AdcDriver g_adcDriver; 

static constexpr AdcReference kAdcRef = AdcReference::AVCC; 
static constexpr AdcPrescaler kAdcPrescaler = AdcPrescaler::DIV128; 
static constexpr uint8_t kAdcDefaultChannel = 0; 


extern Adc g_adcManagerInstance;


void Adc::Init()
{
    g_adcDriver.Init(kAdcRef, kAdcPrescaler);
}

uint16_t Adc::ReadAdcValue(uint8_t channel)
{
    AdcStatus status = g_adcDriver.ReadChannel(channel);
    
    if (status.IsSuccess())
    {
        return status.GetValue();
    }

    UsartDriver::send("ERR: ADC read failed.\r\n");
    return 0;
}

void Adc::Handle()
{
    uint16_t raw_value = ReadAdcValue(kAdcDefaultChannel);
    
    static constexpr uint32_t kVrefMv = 5000UL;
    static constexpr uint16_t kAdcMax = 1023; 
    
    uint32_t voltage_mv = ((uint32_t)raw_value * kVrefMv) / kAdcMax; 
    
    char buffer[50];
    
    snprintf(buffer, sizeof(buffer), "AdcData -> %lu mV (Raw: %hu)\r\n", voltage_mv, raw_value);
    
    UsartDriver::send(buffer);
}