#ifndef LIGHT_SENSOR_MANAGER_HPP
#define LIGHT_SENSOR_MANAGER_HPP

#include "app/adc_manager.hpp"
#include "config/pin_config.hpp"
#include <stdint.h>

class LightSensorManager
{
public:
    static void init();
    static bool isDark(); 
    static uint16_t readValue();
    
private:
    static constexpr uint16_t DARK_THRESHOLD_LOW = 250;   
    static constexpr uint16_t DARK_THRESHOLD_HIGH = 420;
    static bool lastDarkState;
};

#endif