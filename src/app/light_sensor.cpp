#include "app/light_sensor.hpp"
#include "config/pin_config.hpp"

bool LightSensor::last_dark_state_ = false;
Adc* LightSensor::adc_instance_ = nullptr;

void LightSensor::Init(Adc* adc)
{
    adc_instance_ = adc;
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
    if (adc_instance_ != nullptr)
    {
        return adc_instance_->ReadAdcValue(LIGHT_SENSOR_ADC_CHANNEL);
    }
    return 0;
}
