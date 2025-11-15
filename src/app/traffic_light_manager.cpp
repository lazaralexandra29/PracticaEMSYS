#include "app/traffic_light_manager.hpp"
#include "drivers/usart_driver.hpp"
#include <stdio.h>
#include <string.h>

TrafficLightManager* TrafficLightManager::instance = nullptr;

TrafficLightManager::TrafficLightManager(
    volatile uint8_t* leftRedPort, uint8_t leftRedPin,
    volatile uint8_t* leftYellowPort, uint8_t leftYellowPin,
    volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
    volatile uint8_t* rightRedPort, uint8_t rightRedPin,
    volatile uint8_t* rightYellowPort, uint8_t rightYellowPin,
    volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
) : leftRed(leftRedPort, leftRedPin),
    leftYellow(leftYellowPort, leftYellowPin),
    leftGreen(leftGreenPort, leftGreenPin),
    rightRed(rightRedPort, rightRedPin),
    rightYellow(rightYellowPort, rightYellowPin),
    rightGreen(rightGreenPort, rightGreenPin),
    currentState(TrafficLightState::RED)
{
}

void TrafficLightManager::init()
{
    leftRed.init();
    leftYellow.init();
    leftGreen.init();
    
    rightRed.init();
    rightYellow.init();
    rightGreen.init();
    
    setState(TrafficLightState::RED);
    
    registerInstance(this);
}

void TrafficLightManager::setState(TrafficLightState state)
{
    currentState = state;
    updateLights();
}

void TrafficLightManager::toggleYellowLights()
{
    leftYellow.toggle();
    rightYellow.toggle();
}

void TrafficLightManager::updateLights()
{
    leftRed.setState(false);
    leftYellow.setState(false);
    leftGreen.setState(false);
    
    rightRed.setState(false);
    rightYellow.setState(false);
    rightGreen.setState(false);
    
    switch (currentState)
    {
        case TrafficLightState::RED:
            leftRed.setState(true);
            rightRed.setState(true);
            break;
            
        case TrafficLightState::YELLOW:
            leftYellow.setState(true);
            rightYellow.setState(true);
            break;
            
        case TrafficLightState::GREEN:
            leftGreen.setState(true);
            rightGreen.setState(true);
            break;
    }
}

const char* TrafficLightManager::getStateString() const
{
    switch (currentState)
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

void TrafficLightManager::reportState()
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
        "[TRAFFIC] State: %s | Left: %s | Right: %s\r\n",
        getStateString(), getStateString(), getStateString());
    UsartDriver::send(buffer);
}

void TrafficLightManager::registerInstance(TrafficLightManager* inst)
{
    instance = inst;
}

void TrafficLightManager::handleSetState(const char* stateStr)
{
    if (instance == nullptr)
    {
        UsartDriver::send("ERR: Traffic lights not initialized.\r\n");
        return;
    }
    
    TrafficLightState newState;
    
    if (strcmp(stateStr, "RED") == 0 || strcmp(stateStr, "red") == 0)
    {
        newState = TrafficLightState::RED;
    }
    else if (strcmp(stateStr, "YELLOW") == 0 || strcmp(stateStr, "yellow") == 0)
    {
        newState = TrafficLightState::YELLOW;
    }
    else if (strcmp(stateStr, "GREEN") == 0 || strcmp(stateStr, "green") == 0)
    {
        newState = TrafficLightState::GREEN;
    }
    else
    {
        UsartDriver::send("ERR: Invalid state. Use RED, YELLOW, or GREEN.\r\n");
        return;
    }
    
    instance->setState(newState);
    instance->reportState();
}

void TrafficLightManager::handleGetState()
{
    if (instance == nullptr)
    {
        UsartDriver::send("ERR: Traffic lights not initialized.\r\n");
        return;
    }
    
    instance->reportState();
}