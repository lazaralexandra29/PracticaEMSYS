#include "app/pedestrian_button.hpp"
#include "app/uart_interface.hpp"
#include <avr/interrupt.h>

PedestrianButton::PedestrianButton(
    Button* right_button,
    Button* left_button,
    TrafficLight* traffic_lights,
    PedestrianLight* pedestrian_lights,
    Buzzer* buzzers,
    TimerDriver* timer_driver,
    IUartInterface* uart_interface)
    :
    right_button_(right_button),
    left_button_(left_button),
    traffic_lights_(traffic_lights),
    pedestrian_lights_(pedestrian_lights),
    buzzers_(buzzers),
    timer_driver_(timer_driver),
    uart_interface_(uart_interface),
    sequence_state_(PedestrianSequenceState::IDLE),
    buttons_enabled_(true),
    right_pressed_flag_(false),
    left_pressed_flag_(false),
    phase_ticks_(0),
    tick_timer_id_(kInvalidTimerId),
    night_toggle_timer_id_(kInvalidTimerId)
{
}

void PedestrianButton::Init()
{
    PedestrianButtonEventRouter::RegisterHandler(this);

    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);

    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC10);

    EIMSK |= (1 << INT0);
    EIMSK |= (1 << INT1);

    sei();

    tick_timer_id_ = timer_driver_->CreateTimerSoftware();
    if (tick_timer_id_ < 8) 
    {
        timer_driver_->RegisterPeriodicCallback(tick_timer_id_, PedestrianButtonEventRouter::HandleSequenceTick, kTickMs);
    }
}

void PedestrianButton::SetButtonsEnabled(bool enabled)
{
    buttons_enabled_ = enabled;
    if (enabled)
    {
        EIMSK |= (1 << INT0) | (1 << INT1);
    }
    else
    {
        EIMSK &= ~((1 << INT0) | (1 << INT1));
    }
}

bool PedestrianButton::IsSequenceActive() const
{
    return sequence_state_ != PedestrianSequenceState::IDLE;
}

bool PedestrianButton::IsNightMode() const
{
    return (night_toggle_timer_id_ < 8) && (night_toggle_timer_id_ != kInvalidTimerId);
}

void PedestrianButton::PauseNightToggle()
{
    if (night_toggle_timer_id_ < 8)
    {
        timer_driver_->UnregisterPeriodicCallback(night_toggle_timer_id_);
    }
}

void PedestrianButton::ResumeNightToggle()
{
    if (night_toggle_timer_id_ < 8)
    {
        // Reînregistrează callback-ul pentru toggle noapte
        timer_driver_->RegisterPeriodicCallback(
            night_toggle_timer_id_, 
            PedestrianButtonEventRouter::HandleNightToggle, 
            kNightTogglePeriodMs
        );
    }
}

void PedestrianButton::OnRightButtonInterrupt()
{
    right_pressed_flag_ = true;
}

void PedestrianButton::OnLeftButtonInterrupt()
{
    left_pressed_flag_ = true;
}

void PedestrianButton::OnSequenceTick()
{
    ProcessSequenceTick();
}

void PedestrianButton::OnNightToggle()
{
    if (traffic_lights_ != nullptr)
    {
        traffic_lights_->ToggleYellowLights();
    }
}

void PedestrianButton::Update()
{
    if (!buttons_enabled_) return;

    if (right_pressed_flag_)
    {
        right_pressed_flag_ = false;
        Log(PedestrianLogType::kInfo, "Right button pressed.");
        if (sequence_state_ == PedestrianSequenceState::IDLE)
        {
            StartSequence();
        }
    }

    if (left_pressed_flag_)
    {
        left_pressed_flag_ = false;
        Log(PedestrianLogType::kInfo, "Left button pressed.");
        if (sequence_state_ == PedestrianSequenceState::IDLE)
        {
            StartSequence();
        }
    }
}

void PedestrianButton::StartSequence()
{
    if (traffic_lights_ == nullptr || pedestrian_lights_ == nullptr || buzzers_ == nullptr)
    {
        Log(PedestrianLogType::kError, "Dependencies missing. Cannot start sequence.");
        return;
    }

    traffic_lights_->SetState(TrafficLightState::GREEN);
    pedestrian_lights_->SetState(PedestrianLightState::RED);
    buzzers_->SetState(false);

    EnterState(PedestrianSequenceState::WAIT_GREEN, 3000);
}

void PedestrianButton::EnterState(PedestrianSequenceState state, uint32_t duration_ms)
{
    sequence_state_ = state;
    phase_ticks_ = (duration_ms + kTickMs - 1) / kTickMs;

    switch (state)
    {
        case PedestrianSequenceState::WAIT_GREEN:
            traffic_lights_->SetState(TrafficLightState::GREEN);
            pedestrian_lights_->SetState(PedestrianLightState::RED);
            buzzers_->SetState(false);
            Log(PedestrianLogType::kInfo, "Traffic -> GREEN, Pedestrians -> RED.");
            break;

        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            traffic_lights_->SetState(TrafficLightState::YELLOW);
            pedestrian_lights_->SetState(PedestrianLightState::RED);
            buzzers_->SetState(false);
            Log(PedestrianLogType::kInfo, "Traffic -> YELLOW, Pedestrians -> RED.");
            break;

        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            traffic_lights_->SetState(TrafficLightState::RED);
            pedestrian_lights_->SetState(PedestrianLightState::GREEN);
            buzzers_->SetState(true);
            Log(PedestrianLogType::kInfo, "Traffic -> RED, Pedestrians -> GREEN.");
            break;

        case PedestrianSequenceState::PEDESTRIAN_BLINK:
            pedestrian_lights_->SetState(PedestrianLightState::BLINKING);
            Log(PedestrianLogType::kInfo, "Pedestrians -> GREEN (blinking). Traffic remains RED.");
            break;

        default: break;
    }
}

void PedestrianButton::ProcessSequenceTick()
{
    if (sequence_state_ == PedestrianSequenceState::IDLE)
    {
        return;
    }

    if (phase_ticks_ > 0)
    {
        phase_ticks_--;
    }

    switch (sequence_state_)
    {
        case PedestrianSequenceState::WAIT_GREEN:
            if (phase_ticks_ == 0)
            {
                EnterState(PedestrianSequenceState::TRANSITION_TO_YELLOW, 3000);
            }
            break;

        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            if (phase_ticks_ == 0)
            {
                EnterState(PedestrianSequenceState::PEDESTRIAN_CROSSING, 7000);
            }
            break;

        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            buzzers_->Toggle();
            if (phase_ticks_ == 0)
            {
                EnterState(PedestrianSequenceState::PEDESTRIAN_BLINK, 2000);
            }
            break;

        case PedestrianSequenceState::PEDESTRIAN_BLINK:
            pedestrian_lights_->UpdateBlink();
            buzzers_->Toggle(); 
            if (phase_ticks_ == 0)
            {
                sequence_state_ = PedestrianSequenceState::IDLE;
                traffic_lights_->SetState(TrafficLightState::GREEN);
                pedestrian_lights_->SetState(PedestrianLightState::RED);
                buzzers_->SetState(false); 
                Log(PedestrianLogType::kInfo, "Sequence finished. Traffic -> GREEN, Pedestrians -> RED.");
            }
            break;

        default: break;
    }
}

void PedestrianButton::HandleNightMode()
{
    sequence_state_ = PedestrianSequenceState::IDLE;
    phase_ticks_ = 0;

    if (buzzers_ != nullptr)
    {
        buzzers_->SetState(false);
    }

    if (pedestrian_lights_ != nullptr)
    {
        pedestrian_lights_->TurnOffAll();
    }

    if (traffic_lights_ != nullptr)
    {
        traffic_lights_->SetState(TrafficLightState::YELLOW);
    }
    Log(PedestrianLogType::kInfo, "Night mode: Traffic -> YELLOW (blinking), Pedestrians OFF.");

    if (night_toggle_timer_id_ < 8)
    {
        timer_driver_->UnregisterPeriodicCallback(night_toggle_timer_id_);
    }

    night_toggle_timer_id_ = timer_driver_->CreateTimerSoftware();
    if (night_toggle_timer_id_ < 8)
    {
        timer_driver_->RegisterPeriodicCallback(
            night_toggle_timer_id_, 
            PedestrianButtonEventRouter::HandleNightToggle, 
            kNightTogglePeriodMs
        );
    }

    if (uart_interface_ != nullptr)
    {
        uart_interface_->Send("[NIGHT] Night mode activated: Yellow lights blinking (500ms), all other lights OFF.\r\n");
    }
}

void PedestrianButton::HandleDayMode()
{
    if (night_toggle_timer_id_ < 8)
    {
        timer_driver_->UnregisterPeriodicCallback(night_toggle_timer_id_);
        night_toggle_timer_id_ = kInvalidTimerId;
    }

    sequence_state_ = PedestrianSequenceState::IDLE;
    phase_ticks_ = 0;

    if (traffic_lights_ != nullptr)
    {
        traffic_lights_->SetState(TrafficLightState::GREEN);
    }
    if (pedestrian_lights_ != nullptr)
    {
        pedestrian_lights_->SetState(PedestrianLightState::RED);
    }
    if (buzzers_ != nullptr)
    {
        buzzers_->SetState(false);
    }

    SetButtonsEnabled(true);
    Log(PedestrianLogType::kInfo, "Day mode: Traffic -> GREEN, Pedestrians -> RED.");

    if (uart_interface_ != nullptr)
    {
        uart_interface_->Send("[DAY] Night mode deactivated. Normal operation restored (green for cars, red for pedestrians).\r\n");
    }
}

ISR(INT0_vect)
{
    PedestrianButtonEventRouter::HandleRightButtonInterrupt();
}

ISR(INT1_vect)
{
    PedestrianButtonEventRouter::HandleLeftButtonInterrupt();
}

void PedestrianButton::Log(PedestrianLogType log_type, const char* message) const
{
    if (uart_interface_ == nullptr)
    {
        return;
    }

    uart_interface_->Send(GetLogPrefix(log_type));
    uart_interface_->Send(message);
    uart_interface_->Send("\r\n");
}

const char* PedestrianButton::GetLogPrefix(PedestrianLogType log_type) const
{
    switch (log_type)
    {
        case PedestrianLogType::kInfo: return "[INFO] ";
        case PedestrianLogType::kDebug: return "[DEBUG] ";
        case PedestrianLogType::kWarning: return "[WARN] ";
        case PedestrianLogType::kError: return "[ERROR] ";
        default: return "[INFO] ";
    }
}
