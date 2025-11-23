#ifndef PEDESTRIAN_BUTTON_HPP
#define PEDESTRIAN_BUTTON_HPP

#include "app/button.hpp"
#include "app/traffic_light.hpp"
#include "app/pedestrian_light.hpp"
#include "app/buzzer.hpp"
#include "app/pedestrian_button_router.hpp"
#include "app/hardware_uart_interface.hpp"
#include "interfaces/ilogger.hpp"
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

class PedestrianButton : public PedestrianButtonEventHandler
{
public:
    PedestrianButton(Button* right_button,
                     Button* left_button,
                     TrafficLight* traffic_lights,
                     PedestrianLight* pedestrian_lights,
                     Buzzer* buzzers,
                     TimerDriver* timer_driver,
                     IUartInterface* uart_interface,
                     ILogger* logger = nullptr);

    void Init();
    void Update();
    void SetButtonsEnabled(bool enabled);
    void HandleNightMode();
    void HandleDayMode();
    
    bool IsSequenceActive() const;
    bool IsNightMode() const;
    void PauseNightToggle();
    void ResumeNightToggle();

    void OnRightButtonInterrupt() override;
    void OnLeftButtonInterrupt() override;
    void OnSequenceTick() override;
    void OnNightToggle() override;

private:
    static constexpr uint16_t kTickMs = 100;
    static constexpr uint32_t kNightTogglePeriodMs = 500;
    static constexpr uint8_t kInvalidTimerId = 0xFF;

    Button* right_button_;
    Button* left_button_;
    TrafficLight* traffic_lights_;
    PedestrianLight* pedestrian_lights_;
    Buzzer* buzzers_;
    TimerDriver* timer_driver_;
    IUartInterface* uart_interface_;
    ILogger* logger_;

    PedestrianSequenceState sequence_state_;
    bool buttons_enabled_;

    volatile bool right_pressed_flag_;
    volatile bool left_pressed_flag_;

    uint32_t phase_ticks_;
    uint8_t tick_timer_id_;
    uint8_t night_toggle_timer_id_;

    void StartSequence();
    void EnterState(PedestrianSequenceState state, uint32_t duration_ms);
    void ProcessSequenceTick();
};

#endif