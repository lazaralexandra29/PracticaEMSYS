#include "app/light_sensor_manager.hpp"
#include "config/pin_config.hpp"

bool LightSensorManager::lastDarkState = false;

void LightSensorManager::init()
{
    uint16_t value = readValue();

    lastDarkState = (value < 500); 
}

bool LightSensorManager::isDark()
{
    uint16_t value = readValue();
    
    if (lastDarkState)
    {
        if (value >= DARK_THRESHOLD_HIGH)
        {
            lastDarkState = false;
            return false;
        }
        return true;
    }
    else
    {
        if (value < DARK_THRESHOLD_LOW)
        {
            lastDarkState = true;
            return true;  
        }
        return false;
    }
}

uint16_t LightSensorManager::readValue()
{
    return AdcManager::readAdcValue(LIGHT_SENSOR_ADC_CHANNEL);
}