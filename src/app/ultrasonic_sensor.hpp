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
    static constexpr uint32_t kTriggerPulseUs = 10;
    static constexpr uint32_t kTriggerDelayUs = 2;
    
    void InitMicrosecondTimer();
    void StartMicrosecondTimer();
    void StopMicrosecondTimer();
    uint32_t GetMicrosecondCounter();
    void WaitMicroseconds(uint32_t us);
    
    GpioDriver gpio_;
    PinDescription trig_pin_;
    PinDescription echo_pin_;
};

#endif

