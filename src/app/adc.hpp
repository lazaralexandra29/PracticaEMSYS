#ifndef ADC_HPP
#define ADC_HPP

#include "drivers/adc_driver.hpp" 
#include <stdint.h>

class Adc 
{
public:
    static void Init(); 
    static void Handle(); 
    static uint16_t ReadAdcValue(uint8_t channel);
};

#endif 