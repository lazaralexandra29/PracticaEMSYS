#include "app/adc.hpp"
#include "drivers/adc_driver.hpp"
#include "app/logger.hpp"
#include <stdio.h>
#include <stdint.h>

extern AdcDriver g_adcDriver;

static constexpr AdcReference kAdcRef = AdcReference::AVCC;
static constexpr AdcPrescaler kAdcPrescaler = AdcPrescaler::DIV128;
static constexpr uint8_t kAdcDefaultChannel = 0;

ILogger* Adc::logger_ = nullptr;

void Adc::Init(ILogger* logger)
{
    g_adcDriver.Init(kAdcRef, kAdcPrescaler);
    SetLogger(logger);
}

void Adc::SetLogger(ILogger* logger)
{
    logger_ = logger;
}

ILogger* Adc::GetLogger()
{
    if (logger_ == nullptr)
    {
        return static_cast<ILogger*>(Logger::GetInstance());
    }
    return logger_;
}

uint16_t Adc::ReadAdcValue(uint8_t channel)
{
    AdcStatus status = g_adcDriver.ReadChannel(channel);

    if (status.IsSuccess())
    {
        return status.GetValue();
    }

    GetLogger()->LogError("ADC read failed.");
    return 0;
}

