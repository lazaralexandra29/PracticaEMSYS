#include "app/ultrasonic_sensor.hpp"
#include <util/delay.h>

UltrasonicSensor::UltrasonicSensor(volatile uint8_t* trig_port,
                                   uint8_t trig_pin,
                                   volatile uint8_t* echo_port,
                                   uint8_t echo_pin)
    : gpio_(),
      trig_pin_(trig_port, trig_pin),
      echo_pin_(echo_port, echo_pin)
{
}

void UltrasonicSensor::Init()
{
    gpio_.SetPinDirection(trig_pin_, PinDirection::OUT);
    gpio_.SetPinValue(trig_pin_, PinValue::LOW);
    gpio_.SetPinDirection(echo_pin_, PinDirection::IN);
}

bool UltrasonicSensor::MeasureDistanceCm(uint16_t& distance_cm)
{
    gpio_.SetPinValue(trig_pin_, PinValue::LOW);
    _delay_us(2);
    gpio_.SetPinValue(trig_pin_, PinValue::HIGH);
    _delay_us(10);
    gpio_.SetPinValue(trig_pin_, PinValue::LOW);

    PinValue pin_state = PinValue::LOW;
    uint32_t timeout = kEchoTimeoutUs;
    while (timeout--)
    {
        gpio_.ReadPinValue(echo_pin_, pin_state);
        if (pin_state == PinValue::HIGH)
        {
            break;
        }
        _delay_us(1);
    }

    if (timeout == 0)
    {
        return false;
    }

    uint32_t pulse_length = 0;
    while (pulse_length < kEchoTimeoutUs)
    {
        gpio_.ReadPinValue(echo_pin_, pin_state);
        if (pin_state == PinValue::LOW)
        {
            break;
        }

        _delay_us(1);
        pulse_length++;
    }

    if (pulse_length >= kEchoTimeoutUs)
    {
        return false;
    }

    distance_cm = static_cast<uint16_t>(pulse_length / 10);
    return true;
}

