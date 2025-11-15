#include "app/pedestrian_light_manager.hpp"
#include "drivers/usart_driver.hpp"

PedestrianLightManager::PedestrianLightManager(
    volatile uint8_t* leftRedPort, uint8_t leftRedPin,
    volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
    volatile uint8_t* rightRedPort, uint8_t rightRedPin,
    volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
) : leftRed(leftRedPort, leftRedPin),
    leftGreen(leftGreenPort, leftGreenPin),
    rightRed(rightRedPort, rightRedPin),
    rightGreen(rightGreenPort, rightGreenPin),
    currentState(PedestrianLightState::RED),
    blinkState(false)
{
}

void PedestrianLightManager::init()
{
    leftRed.init();
    leftGreen.init();
    rightRed.init();
    rightGreen.init();
    
    setState(PedestrianLightState::RED);
}

void PedestrianLightManager::setState(PedestrianLightState state)
{
    currentState = state;
    blinkState = false;
    updateLights();
}

void PedestrianLightManager::updateBlink()
{
    if (currentState == PedestrianLightState::BLINKING)
    {
        blinkState = !blinkState;
        updateLights();
    }
}

void PedestrianLightManager::turnOffAll()
{
    leftRed.setState(false);
    leftGreen.setState(false);
    rightRed.setState(false);
    rightGreen.setState(false);
}

void PedestrianLightManager::updateLights()
{
    leftRed.setState(false);
    leftGreen.setState(false);
    rightRed.setState(false);
    rightGreen.setState(false);
    
    switch (currentState)
    {
        case PedestrianLightState::RED:
            leftRed.setState(true);
            rightRed.setState(true);
            break;
            
        case PedestrianLightState::GREEN:
            leftGreen.setState(true);
            rightGreen.setState(true);
            break;
            
        case PedestrianLightState::BLINKING:
            if (blinkState)
            {
                leftGreen.setState(true);
                rightGreen.setState(true);
            }
            break;
    }
}