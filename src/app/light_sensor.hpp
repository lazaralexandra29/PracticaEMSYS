#ifndef LIGHT_SENSOR_HPP
#define LIGHT_SENSOR_HPP

#include "app/adc.hpp"
#include "config/pin_config.hpp"
#include <stdint.h>

class LightSensor
{
public:
    static void Init();
    static bool IsDark(); 
    static uint16_t ReadValue();
    
private:
    static constexpr uint16_t kDarkThresholdLow = 250;   
    static constexpr uint16_t kDarkThresholdHigh = 420;
    static bool last_dark_state_;
};

#endif

