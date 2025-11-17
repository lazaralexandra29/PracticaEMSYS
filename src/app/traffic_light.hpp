#ifndef TRAFFIC_LIGHT_HPP
#define TRAFFIC_LIGHT_HPP

#include "app/led.hpp"
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
        volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
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
    
    static void RegisterInstance(TrafficLight* instance);
    static void HandleSetState(const char* state_str);
    static void HandleGetState();

private:
    Led left_red_;
    Led left_yellow_;
    Led left_green_;
    
    Led right_red_;
    Led right_yellow_;
    Led right_green_;
    
    TrafficLightState current_state_;
    
    void UpdateLights();
    
    static TrafficLight* instance_;
};

#endif