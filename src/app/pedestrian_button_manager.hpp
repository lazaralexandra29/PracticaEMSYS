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
    WAIT_GREEN = 1,
    TRANSITION_TO_YELLOW = 2,
    PEDESTRIAN_CROSSING = 4,
    PEDESTRIAN_BLINK = 5
};

class PedestrianButtonManager
{
public:
    PedestrianButtonManager(ButtonManager* buttonRight,
                            ButtonManager* buttonLeft,
                            TrafficLightManager* trafficLights,
                            PedestrianLightManager* pedestrianLights,
                            BuzzerManager* buzzers,
                            TimerDriver* timerDriver);

    void init();
    void update();

    void setButtonsEnabled(bool enabled);

    static void handleInterruptRight();
    static void handleInterruptLeft();
    static void onSequenceTick(); 

    static void handleNight();
    static void handleDay();

private:
    ButtonManager* buttonRight;
    ButtonManager* buttonLeft;
    TrafficLightManager* trafficLights;
    PedestrianLightManager* pedestrianLights;
    BuzzerManager* buzzers;
    TimerDriver* timerDriver;

    PedestrianSequenceState sequenceState;
    bool buttonsEnabled;

    volatile bool rightPressedFlag;
    volatile bool leftPressedFlag;

    static constexpr uint16_t tickMs = 100;
    uint32_t phaseTicks;
    uint8_t tickTimerId;

    void startSequence();
    void enterState(PedestrianSequenceState st, uint32_t durationMs);
    void sequenceTick();
};

#endif