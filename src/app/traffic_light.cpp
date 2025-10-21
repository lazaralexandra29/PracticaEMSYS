#include <app/traffic_light.h>
#include <drivers/gpio_driver.h>
#include <config/main_defines.h>

extern GpioDriver myGpio;

void TrafficLight_CarInit() {

    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED_CAR_RED_PIN, PinDir::OUT);
    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED_CAR_YELLOW_PIN, PinDir::OUT);
    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED_CAR_GREEN_PIN, PinDir::OUT);

    myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_RED_PIN, PinVal::LOW);
    myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_YELLOW_PIN, PinVal::LOW);
    myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_GREEN_PIN, PinVal::LOW);
}