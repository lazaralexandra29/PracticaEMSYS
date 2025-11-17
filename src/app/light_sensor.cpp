#include "app/light_sensor.hpp"
#include "config/pin_config.hpp"

bool LightSensor::last_dark_state_ = false;

void LightSensor::Init()
{
    uint16_t value = ReadValue();

    last_dark_state_ = (value < 500); 
}

bool LightSensor::IsDark()
{
    uint16_t value = ReadValue();
    
    if (last_dark_state_)
    {
        if (value >= kDarkThresholdHigh)
        {
            last_dark_state_ = false;
            return false;
        }
        return true;
    }
    else
    {
        if (value < kDarkThresholdLow)
        {
            last_dark_state_ = true;
            return true;  
        }
        return false;
    }
}

uint16_t LightSensor::ReadValue()
{
    return Adc::ReadAdcValue(LIGHT_SENSOR_ADC_CHANNEL);
}

