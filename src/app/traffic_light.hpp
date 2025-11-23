#ifndef TRAFFIC_LIGHT_HPP
#define TRAFFIC_LIGHT_HPP

#include "app/led.hpp"
#include "interfaces/ilogger.hpp"
#include <stdint.h>

enum class TrafficLightState : uint8_t
{
    RED = 0,
    YELLOW = 1,
    GREEN = 2
};

class TrafficLight
{
public:
    TrafficLight(
        volatile uint8_t* leftRedPort, uint8_t leftRedPin,
        volatile uint8_t* leftYellowPort, uint8_t leftYellowPin,
        volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
        volatile uint8_t* rightRedPort, uint8_t rightRedPin,
        volatile uint8_t* rightYellowPort, uint8_t rightYellowPin,
        volatile uint8_t* rightGreenPort, uint8_t rightGreenPin,
        ILogger& logger
    );

    void Init();
    void SetState(TrafficLightState state);
    void ToggleYellowLights();
    
    TrafficLightState GetState() const 
    {
        return current_state_; 
    }
    const char* GetStateString() const;
    
    void ReportState();
    
    ILogger& GetLogger() 
    { 
        return logger_; 
    }

private:
    Led left_red_;
    Led left_yellow_;
    Led left_green_;
    
    Led right_red_;
    Led right_yellow_;
    Led right_green_;
    
    TrafficLightState current_state_;
    ILogger& logger_;
    
    void UpdateLights();

    bool red_shared_;
    bool yellow_shared_;
    bool green_shared_;
};

#endif
