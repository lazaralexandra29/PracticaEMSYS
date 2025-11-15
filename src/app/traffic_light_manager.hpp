#ifndef TRAFFIC_LIGHT_MANAGER_HPP
#define TRAFFIC_LIGHT_MANAGER_HPP

#include "app/led_manager.hpp"
#include <stdint.h>

enum class TrafficLightState : uint8_t
{
    RED = 0,
    YELLOW = 1,
    GREEN = 2
};

class TrafficLightManager
{
public:
    TrafficLightManager(
        volatile uint8_t* leftRedPort, uint8_t leftRedPin,
        volatile uint8_t* leftYellowPort, uint8_t leftYellowPin,
        volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
        volatile uint8_t* rightRedPort, uint8_t rightRedPin,
        volatile uint8_t* rightYellowPort, uint8_t rightYellowPin,
        volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
    );

    void init();
    void setState(TrafficLightState state);
    void toggleYellowLights();
    
    TrafficLightState getState() const 
    {
        return currentState; 
    }
    const char* getStateString() const;
    
    void reportState();
    
    static void registerInstance(TrafficLightManager* instance);
    static void handleSetState(const char* stateStr);
    static void handleGetState();

private:
    LedManager leftRed;
    LedManager leftYellow;
    LedManager leftGreen;
    
    LedManager rightRed;
    LedManager rightYellow;
    LedManager rightGreen;
    
    TrafficLightState currentState;
    
    void updateLights();
    
    static TrafficLightManager* instance;
};

#endif

