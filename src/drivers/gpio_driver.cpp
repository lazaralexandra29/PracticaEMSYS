#include <drivers/gpio_driver.h>

int8_t GpioDriver::SetPinDirection(volatile uint8_t *port, uint8_t pin, PinDir dir){
    if (pin > 7) return -1;
    if (dir == PinDir::OUT){
        *port |= (1 << pin);
    } else if (dir == PinDir::IN){
        *port &= ~(1 << pin);
    } else{
        return -2;
    }
    return 0;
}

int8_t GpioDriver::SetPinValue(volatile uint8_t *port, uint8_t pin, PinVal value){
    if (pin > 7) return -1;
    if (value == PinVal::HIGH){
        *port |= (1 << pin);
    } else if (value == PinVal::LOW){
        *port &= ~(1 << pin);
    } else {
        return -2;
    }
    return 0;
}

int8_t GpioDriver::ReadPinValue(volatile uint8_t *port, uint8_t pin, PinVal &value){
    if (pin > 7) return -1;
    value = ((*port & (1 << pin)) ? PinVal::HIGH : PinVal::LOW);
    return 0;
}