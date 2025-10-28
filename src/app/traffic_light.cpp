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

void TrafficLight_SetCarState(TrafficLightState state) {
    switch (state) {
        case TL_STATE_GREEN:
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_GREEN_PIN, PinVal::HIGH);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_RED_PIN, PinVal::LOW);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_YELLOW_PIN, PinVal::LOW);
            break;

        case TL_STATE_YELLOW:
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_YELLOW_PIN, PinVal::HIGH);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_RED_PIN, PinVal::LOW);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_GREEN_PIN, PinVal::LOW);
            break;

        case TL_STATE_RED:
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_RED_PIN, PinVal::HIGH);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_YELLOW_PIN, PinVal::LOW);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_GREEN_PIN, PinVal::LOW);
            break;

        default:
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_RED_PIN, PinVal::LOW);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_YELLOW_PIN, PinVal::LOW);
            myGpio.SetPinValue(GPIO_PORTB_PORT, LED_CAR_GREEN_PIN, PinVal::LOW);
            break;
    }
}