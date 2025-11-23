#include "app/adc.hpp"
#include "app/logger.hpp"
#include "drivers/adc_driver.hpp"
#include <stdio.h>
#include <stdint.h>

extern AdcDriver g_adcDriver;

static constexpr AdcReference kAdcRef = AdcReference::AVCC;
static constexpr AdcPrescaler kAdcPrescaler = AdcPrescaler::DIV128;
static constexpr uint8_t kAdcDefaultChannel = 0;

Adc::Adc(ILogger* logger)
    : logger_(logger != nullptr ? logger : Logger::GetInstance())
{
}

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

    logger_->Log(LogLevel::ERROR, "ADC read failed.");
    return 0;
}
