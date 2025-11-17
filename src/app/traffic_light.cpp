#include "app/traffic_light.hpp"
#include "drivers/usart_driver.hpp"
#include <stdio.h>
#include <string.h>

TrafficLight* TrafficLight::instance_ = nullptr;

TrafficLight::TrafficLight(
    volatile uint8_t* leftRedPort, uint8_t leftRedPin,
    volatile uint8_t* leftYellowPort, uint8_t leftYellowPin,
    volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
    volatile uint8_t* rightRedPort, uint8_t rightRedPin,
    volatile uint8_t* rightYellowPort, uint8_t rightYellowPin,
    volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
) : left_red_(leftRedPort, leftRedPin),
    left_yellow_(leftYellowPort, leftYellowPin),
    left_green_(leftGreenPort, leftGreenPin),
    right_red_(rightRedPort, rightRedPin),
    right_yellow_(rightYellowPort, rightYellowPin),
    right_green_(rightGreenPort, rightGreenPin),
    current_state_(TrafficLightState::RED)
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
    
    RegisterInstance(this);
}

void TrafficLight::SetState(TrafficLightState state)
{
    current_state_ = state;
    UpdateLights();
}

void TrafficLight::ToggleYellowLights()
{
    left_yellow_.Toggle();
    right_yellow_.Toggle();
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
        "[TRAFFIC] State: %s | Left: %s | Right: %s\r\n",
        GetStateString(), GetStateString(), GetStateString());
    UsartDriver::send(buffer);
}

void TrafficLight::RegisterInstance(TrafficLight* inst)
{
    instance_ = inst;
}

void TrafficLight::HandleSetState(const char* state_str)
{
    if (instance_ == nullptr)
    {
        UsartDriver::send("ERR: Traffic lights not initialized.\r\n");
        return;
    }
    
    TrafficLightState new_state;
    
    if (strcmp(state_str, "RED") == 0 || strcmp(state_str, "red") == 0)
    {
        new_state = TrafficLightState::RED;
    }
    else if (strcmp(state_str, "YELLOW") == 0 || strcmp(state_str, "yellow") == 0)
    {
        new_state = TrafficLightState::YELLOW;
    }
    else if (strcmp(state_str, "GREEN") == 0 || strcmp(state_str, "green") == 0)
    {
        new_state = TrafficLightState::GREEN;
    }
    else
    {
        UsartDriver::send("ERR: Invalid state. Use RED, YELLOW, or GREEN.\r\n");
        return;
    }
    
    instance_->SetState(new_state);
    instance_->ReportState();
}

void TrafficLight::HandleGetState()
{
    if (instance_ == nullptr)
    {
        UsartDriver::send("ERR: Traffic lights not initialized.\r\n");
        return;
    }
    
    instance_->ReportState();
}
