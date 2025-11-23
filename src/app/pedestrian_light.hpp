#ifndef PEDESTRIAN_LIGHT_HPP
#define PEDESTRIAN_LIGHT_HPP

#include "app/led.hpp"
#include "interfaces/ilogger.hpp"
#include <stdint.h>

enum class PedestrianLightState : uint8_t
{
    RED = 0,     
    GREEN = 1,   
    BLINKING = 2 
};

class PedestrianLight
{
public:
    PedestrianLight(
        volatile uint8_t* leftRedPort, uint8_t leftRedPin,
        volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
        volatile uint8_t* rightRedPort, uint8_t rightRedPin,
        volatile uint8_t* rightGreenPort, uint8_t rightGreenPin,
        ILogger& logger
    );

    void Init();
    void SetState(PedestrianLightState state);
    void UpdateBlink();
    void TurnOffAll();
    
    PedestrianLightState GetState() const 
    { 
        return current_state_; 
    }

private:
    Led left_red_;
    Led left_green_;
    Led right_red_;
    Led right_green_;
    
    PedestrianLightState current_state_;
    bool blink_state_; 
    
    void UpdateLights();
};

#endif
