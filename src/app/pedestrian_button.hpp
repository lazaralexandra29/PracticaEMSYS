#ifndef PEDESTRIAN_BUTTON_HPP
#define PEDESTRIAN_BUTTON_HPP

#include "app/button.hpp"
#include "app/traffic_light.hpp"
#include "app/pedestrian_light.hpp"
#include "app/buzzer.hpp"
#include "app/pedestrian_button_router.hpp"
#include "app/uart_interface.hpp"
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

enum class PedestrianLogType : uint8_t
{
    kInfo = 0,
    kDebug = 1,
    kWarning = 2,
    kError = 3
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
                     IUartInterface* uart_interface);

    void Init();
    void Update();
    void SetButtonsEnabled(bool enabled);
    void HandleNightMode();
    void HandleDayMode();

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
    void Log(PedestrianLogType log_type, const char* message) const;
    const char* GetLogPrefix(PedestrianLogType log_type) const;
};

#endif