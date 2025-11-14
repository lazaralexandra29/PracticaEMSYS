#ifndef ADC_MANAGER_HPP
#define ADC_MANAGER_HPP

#include "drivers/adc_driver.hpp" 
#include <stdint.h>

class AdcManager 
{
    public:
     static void init(); 
     static void handle(); 
     static uint16_t readAdcValue(uint8_t channel);
};

#endif 