#include <app/pedestrian_crossing.h>
#include <app/traffic_light.h>
#include <drivers/gpio_driver.h>
#include <config/main_defines.h>

extern GpioDriver myGpio;
extern volatile bool tick_500ms;
extern volatile bool button_pressed;

static PedestrianTrafficState current_state;
static uint16_t timer_count = 0;

void SetPedestrianLeds(PinVal red, PinVal green){
    myGpio.SetPinValue(GPIO_PORTB_PORT, LED1_PED_RED, red);
    myGpio.SetPinValue(GPIO_PORTB_PORT, LED2_PED_GREEN, green);
}

void PedestrianCrossing_Init(void){
    TrafficLight_SetCarState(TL_STATE_GREEN);
    SetPedestrianLeds(PinVal::HIGH, PinVal::LOW);

    current_state = STATE_CAR_GREEN_PED_RED;
    timer_count = 0;
}

void  PedestrianCrossing_Run(void){
    if (tick_500ms){
        tick_500ms = false;
        timer_count++;
    }

    switch (current_state){
        case  STATE_CAR_GREEN_PED_RED:
         TrafficLight_SetCarState(TL_STATE_GREEN);
         SetPedestrianLeds(PinVal::HIGH, PinVal::LOW);

         if(button_pressed){
            button_pressed = false;
            timer_count = 0;
            current_state = STATE_PED_WAITING;
         }
        break;

        case STATE_PED_WAITING:
         if(timer_count >= 4){
            TrafficLight_SetCarState(TL_STATE_YELLOW);
            timer_count = 0;
            current_state = STATE_CAR_YELLOW;
         }
        break;

        case STATE_CAR_YELLOW:
         if(timer_count >= 4){
            TrafficLight_SetCarState(TL_STATE_RED);
            timer_count = 0;
            current_state = STATE_CAR_RED_PED_TRANSITION;
         }
        break;

        case STATE_CAR_RED_PED_TRANSITION:
         if(timer_count >= 2){
            SetPedestrianLeds(PinVal::LOW, PinVal::HIGH);
            timer_count = 0;
            current_state = STATE_CAR_RED_PED_GREEN;
         }
        break;

        case STATE_CAR_RED_PED_GREEN:
         if(timer_count >= 16){
            timer_count = 0;
            current_state = STATE_CAR_RED_PED_BLINK;
         }
        break;

        case STATE_CAR_RED_PED_BLINK:
         if(timer_count < 8){
            if(tick_500ms)
            {
                if(timer_count % 2 == 0) {
                    myGpio.PinToggle(GPIO_PORTB_PORT, LED2_PED_GREEN);
                }
            }
         }
         else {
            TrafficLight_SetCarState(TL_STATE_GREEN);
            SetPedestrianLeds(PinVal::HIGH, PinVal::LOW);
            timer_count = 0;
            current_state = STATE_CAR_GREEN_PED_RED;
         }
        break;
    }
}