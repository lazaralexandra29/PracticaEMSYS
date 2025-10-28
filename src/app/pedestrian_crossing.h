#ifndef PEDESTRIAN_CROSSING_H
#define PEDESTRIAN_CROSSING_H

#include <stdbool.h>
#include <app/traffic_light.h>

extern volatile bool tick_500ms;
extern volatile bool button_pressed;

typedef enum {
    STATE_CAR_GREEN_PED_RED,
    STATE_PED_WAITING,
    STATE_CAR_YELLOW,
    STATE_CAR_RED_PED_TRANSITION,
    STATE_CAR_RED_PED_GREEN,
    STATE_CAR_RED_PED_BLINK
} PedestrianTrafficState;

void PedestrianCrossing_Init(void);

void PedestrianCrossing_Run(void);

#endif