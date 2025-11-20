#ifndef ULTRASONIC_SENSOR_HPP
#define ULTRASONIC_SENSOR_HPP

#include "drivers/gpio_driver.hpp"
#include <stdint.h>

class UltrasonicSensor
{
public:
    UltrasonicSensor(volatile uint8_t* trig_port,
                     uint8_t trig_pin,
                     volatile uint8_t* echo_port,
                     uint8_t echo_pin);

    void Init();
    bool MeasureDistanceCm(uint16_t& distance_cm);

private:
    static constexpr uint32_t kEchoTimeoutUs = 30000;
    GpioDriver gpio_;
    PinDescription trig_pin_;
    PinDescription echo_pin_;
};

#endif

