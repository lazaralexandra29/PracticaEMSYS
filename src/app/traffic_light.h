#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdint.h>

typedef enum{
    TL_STATE_GREEN,
    TL_STATE_YELLOW,
    TL_STATE_RED
} TrafficLightState;

void TrafficLight_CarInit(void);

void TrafficLight_SetCarState(TrafficLightState state);

#endif