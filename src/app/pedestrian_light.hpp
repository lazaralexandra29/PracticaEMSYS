#ifndef PEDESTRIAN_LIGHT_HPP
#define PEDESTRIAN_LIGHT_HPP

#include "app/led.hpp"
#include "app/logger.hpp"
#include <stdint.h>

enum class PedestrianLightState : uint8_t
{
    RED = 0,     
    GREEN = 1,   
    BLINKING = 2 
};

/// <summary>
/// Pedestrian light controller with dependency injection for logging
/// </summary>
class PedestrianLight
{
public:
    /// <summary>
    /// Constructor with logger dependency injection
    /// </summary>
    /// <param name="leftRedPort">Left red LED port</param>
    /// <param name="leftRedPin">Left red LED pin</param>
    /// <param name="leftGreenPort">Left green LED port</param>
    /// <param name="leftGreenPin">Left green LED pin</param>
    /// <param name="rightRedPort">Right red LED port</param>
    /// <param name="rightRedPin">Right red LED pin</param>
    /// <param name="rightGreenPort">Right green LED port</param>
    /// <param name="rightGreenPin">Right green LED pin</param>
    /// <param name="logger">Logger instance (nullptr uses singleton)</param>
    PedestrianLight(
        volatile uint8_t* leftRedPort, uint8_t leftRedPin,
        volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
        volatile uint8_t* rightRedPort, uint8_t rightRedPin,
        volatile uint8_t* rightGreenPort, uint8_t rightGreenPin,
        ILogger* logger = nullptr
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