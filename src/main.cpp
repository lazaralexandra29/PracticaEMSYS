#include <avr/io.h>
#include <drivers/gpio_driver.hpp>  

int main() {

    GpioDriver gpioDriver;

    PinDescription led(&PORTB, 5u); //imi da eroare cu const la port, cu volatile nu!!!!

    GpioStatus status = gpioDriver.SetPinDirection(led, PinDirection::OUT);

    if(!status.IsSuccess()) 
    {
        return -1;
    } 

    gpioDriver.SetPinValue(led, PinValue::HIGH);

    return 0; 
}   