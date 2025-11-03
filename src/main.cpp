#include <avr/io.h>
#include <avr/interrupt.h>

#include <drivers/gpio_driver.h>
#include <drivers/timer_driver.h>
#include <drivers/interrupts_driver.h>
#include <drivers/usart_driver.h>
#include <app/pedestrian_crossing.h>
#include <app/traffic_light.h>
#include <config/main_defines.h>

GpioDriver myGpio;
TimerDriver myTimer;

volatile bool tick_500ms = false;
volatile bool button_pressed = false;

void TimerTickCallback() {
    tick_500ms = true;
}

void ButtonPressCallback() {
    button_pressed = true;
}

void InitSystem(){

    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED_CAR_GREEN_PIN, PinDir::OUT);
    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED_CAR_YELLOW_PIN, PinDir::OUT);
    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED_CAR_RED_PIN, PinDir::OUT);
    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED1_PED_RED, PinDir::OUT);   
    myGpio.SetPinDirection(GPIO_PORTB_DDR, LED2_PED_GREEN, PinDir::OUT);

    myGpio.SetPinDirection(GPIO_PORTD_DDR, BUTTON_PIN, PinDir::IN);

    myTimer.Init(1, TimerMode::CTC, Prescaler::DIV_1024);
    myTimer.SetCompareValue(1 , 7812);
    myTimer.RegisterCallback(TimerTickCallback);

    PedestrianCrossing_Init();
}


int main(void){

    InitSystem();

    myInterrupts.EnabelGlobalInterrupts();

    while (1) 
    {
        PedestrianCrossing_Run();
    }
}