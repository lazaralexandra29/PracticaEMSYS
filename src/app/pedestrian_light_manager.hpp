#ifndef PEDESTRIAN_LIGHT_MANAGER_HPP
#define PEDESTRIAN_LIGHT_MANAGER_HPP

#include "app/led_manager.hpp"
#include <stdint.h>

enum class PedestrianLightState : uint8_t
{
    RED = 0,     
    GREEN = 1,   
    BLINKING = 2 
};

class PedestrianLightManager
{
public:
    PedestrianLightManager(
        volatile uint8_t* leftRedPort, uint8_t leftRedPin,
        volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
        volatile uint8_t* rightRedPort, uint8_t rightRedPin,
        volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
    );

    void init();
    void setState(PedestrianLightState state);
    void updateBlink();
    void turnOffAll();
    
    PedestrianLightState getState() const 
    { 
        return currentState; 
    }

private:
    LedManager leftRed;
    LedManager leftGreen;
    LedManager rightRed;
    LedManager rightGreen;
    
    PedestrianLightState currentState;
    bool blinkState; 
    
    void updateLights();
};

#endif