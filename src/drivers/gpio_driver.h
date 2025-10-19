#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

enum class PinDir : uint8_t {
    IN=0,
    OUT=1
};

enum class PinVal : uint8_t {
    LOW = 0,
    HIGH =1
};

class GpioDriver {
    public:
    int8_t SetPinDirection (volatile uint8_t *port, uint8_t pin, PinDir dir);
    int8_t SetPinValue (volatile uint8_t *port, uint8_t pin, PinVal value);
    int8_t ReadPinValue (volatile uint8_t *port, uint8_t pin, PinVal &value);
};

#endif