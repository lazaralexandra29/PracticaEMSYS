#include "app/traffic_light.hpp"
#include <stdio.h>
#include <string.h>

TrafficLight::TrafficLight(
    volatile uint8_t* leftRedPort, uint8_t leftRedPin,
    volatile uint8_t* leftYellowPort, uint8_t leftYellowPin,
    volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
    volatile uint8_t* rightRedPort, uint8_t rightRedPin,
    volatile uint8_t* rightYellowPort, uint8_t rightYellowPin,
    volatile uint8_t* rightGreenPort, uint8_t rightGreenPin,
    ILogger& logger
) : left_red_(leftRedPort, leftRedPin, &logger),
    left_yellow_(leftYellowPort, leftYellowPin, &logger),
    left_green_(leftGreenPort, leftGreenPin, &logger),
    right_red_(rightRedPort, rightRedPin, &logger),
    right_yellow_(rightYellowPort, rightYellowPin, &logger),
    right_green_(rightGreenPort, rightGreenPin, &logger),
    current_state_(TrafficLightState::RED),
    logger_(logger),
    red_shared_((leftRedPort == rightRedPort) && (leftRedPin == rightRedPin)),
    yellow_shared_((leftYellowPort == rightYellowPort) && (leftYellowPin == rightYellowPin)),
    green_shared_((leftGreenPort == rightGreenPort) && (leftGreenPin == rightGreenPin))
{
}

void TrafficLight::Init()
{
    left_red_.Init();
    left_yellow_.Init();
    left_green_.Init();
    
    right_red_.Init();
    right_yellow_.Init();
    right_green_.Init();
    
    SetState(TrafficLightState::RED);
}

void TrafficLight::SetState(TrafficLightState state)
{
    current_state_ = state;
    UpdateLights();
}

void TrafficLight::ToggleYellowLights()
{
    left_yellow_.Toggle();
    if (!yellow_shared_)
    {
        right_yellow_.Toggle();
    }
}

void TrafficLight::UpdateLights()
{
    left_red_.SetState(false);
    left_yellow_.SetState(false);
    left_green_.SetState(false);
    right_red_.SetState(false);
    right_yellow_.SetState(false);
    right_green_.SetState(false);
    
    switch (current_state_)
    {
        case TrafficLightState::RED:
            left_red_.SetState(true);
            right_red_.SetState(true);
            break;
            
        case TrafficLightState::YELLOW:
            left_yellow_.SetState(true);
            right_yellow_.SetState(true);
            break;
            
        case TrafficLightState::GREEN:
            left_green_.SetState(true);
            right_green_.SetState(true);
            break;
            
        default:
            break;
    }
}

const char* TrafficLight::GetStateString() const
{
    switch (current_state_)
    {
        case TrafficLightState::RED:
            return "RED";
        case TrafficLightState::YELLOW:
            return "YELLOW";
        case TrafficLightState::GREEN:
            return "GREEN";
        default:
            return "UNKNOWN";
    }
}

void TrafficLight::ReportState()
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
        "[TRAFFIC] State: %s | Left: %s | Right: %s",
        GetStateString(), GetStateString(), GetStateString());
    
    logger_.Log(LogLevel::INFO, buffer);
}
