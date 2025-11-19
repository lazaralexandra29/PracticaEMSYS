#ifndef ADC_HPP
#define ADC_HPP

#include "drivers/adc_driver.hpp"
#include "app/logger.hpp"
#include <stdint.h>

class Adc
{
public:
    static void Init(ILogger* logger = nullptr);
    static uint16_t ReadAdcValue(uint8_t channel);
    static void SetLogger(ILogger* logger);

private:
    static ILogger* logger_;
    static ILogger* GetLogger();
};

#endif 