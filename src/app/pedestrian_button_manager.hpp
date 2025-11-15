#ifndef PEDESTRIAN_BUTTON_MANAGER_HPP
#define PEDESTRIAN_BUTTON_MANAGER_HPP

#include "app/button_manager.hpp"
#include "app/traffic_light_manager.hpp"
#include "app/pedestrian_light_manager.hpp"
#include "app/buzzer_manager.hpp"
#include "drivers/timer_driver.hpp"
#include <stdint.h>

enum class PedestrianSequenceState : uint8_t
{
    IDLE = 0,           
    TRANSITION_TO_YELLOW = 1,
    TRANSITION_TO_RED = 2,      
    PEDESTRIAN_CROSSING = 3    
};

class PedestrianButtonManager
{
public:
    PedestrianButtonManager(ButtonManager* buttonRight, ButtonManager* buttonLeft,
                           TrafficLightManager* trafficLights, 
                           PedestrianLightManager* pedestrianLights, BuzzerManager* buzzers,
                           TimerDriver* timerDriver);
    
    void init();
    void update();
    void setButtonsEnabled(bool enabled); 
    void startNightMode();  
    void stopNightMode(); 
    
    static void handleInterruptRight();  
    static void handleInterruptLeft();  

    static void onYellowTimeout();
    static void onRedTimeout();
    static void onPedestrianTimeout();
    static void onStatusUpdate(); 
    static void onBuzzerToggle();  // Nu mai e folosit 
    static void onBuzzerBip();     // Nu mai e folosit 
    static void onPedestrianBlink(); // Nu mai e folosit 
    static void onYellowToggle(); 
    
    void logButtonPress(const char* side = "UNKNOWN");
    void logSequenceState();
    void logTimeRemaining();
    void logButtonState();  
    void logFullStatus();   
    void logDetailedStatus(); 
    
    uint16_t getPressCount() const 
    {
        return pressCount; 
    }

    uint16_t getRightPressCount() const 
    { 
        return rightPressCount; 
    }

    uint16_t getLeftPressCount() const 
    { 
        return leftPressCount; 
    }

    bool isButtonPressed() const;
    
    PedestrianSequenceState getSequenceState() const 
    { 
        return sequenceState; 
    }

    uint8_t getSecondsRemaining() const;

    bool arePedestriansCrossing() const 
    { 
        return sequenceState == PedestrianSequenceState::PEDESTRIAN_CROSSING; 
    }
    
    static void handleGetStatus();
    static void handleNight(); 
    static void handleDay();

private:
    ButtonManager* buttonRight;
    ButtonManager* buttonLeft;
    TrafficLightManager* trafficLights;
    PedestrianLightManager* pedestrianLights;
    BuzzerManager* buzzers;
    TimerDriver* timerDriver;
    
    uint16_t pressCount;       
    uint16_t rightPressCount; 
    uint16_t leftPressCount; 
    PedestrianSequenceState sequenceState;
    
    bool buttonRightPressedFlag; 
    bool buttonLeftPressedFlag;   
    bool buttonsEnabled;
    uint8_t secondsRemaining;
    
    uint8_t buzzerToggleTimerId;   // Nu mai e folosit
    uint8_t buzzerBipTimerId;      // Nu mai e folosit 
    uint8_t pedestrianBlinkTimerId; // Nu mai e folosit
    uint8_t yellowToggleTimerId;   // Nu mai e folosit
    bool buzzerBipState;           // Nu mai e folosit

    bool nightModeActive;        
    uint16_t nightToggleCounter;
    
    void startSequence();
    void stopAllTimers();
    void updateSecondsRemaining();
};

#endif