#ifndef ADC_HPP
#define ADC_HPP

#include "drivers/adc_driver.hpp"
#include "interfaces/ilogger.hpp"
#include <stdint.h>

class Adc
{
public:
    Adc(ILogger* logger = nullptr);

    void Init();
    uint16_t ReadAdcValue(uint8_t channel);

private:
    ILogger* logger_;
};

#endif
