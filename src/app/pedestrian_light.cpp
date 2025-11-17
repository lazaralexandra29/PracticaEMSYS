#include "app/pedestrian_light.hpp"
#include "drivers/usart_driver.hpp"

PedestrianLight::PedestrianLight(
    volatile uint8_t* leftRedPort, uint8_t leftRedPin,
    volatile uint8_t* leftGreenPort, uint8_t leftGreenPin,
    volatile uint8_t* rightRedPort, uint8_t rightRedPin,
    volatile uint8_t* rightGreenPort, uint8_t rightGreenPin
) : left_red_(leftRedPort, leftRedPin),
    left_green_(leftGreenPort, leftGreenPin),
    right_red_(rightRedPort, rightRedPin),
    right_green_(rightGreenPort, rightGreenPin),
    current_state_(PedestrianLightState::RED),
    blink_state_(false)
{
}

void PedestrianLight::Init()
{
    left_red_.Init();
    left_green_.Init();
    right_red_.Init();
    right_green_.Init();
    
    SetState(PedestrianLightState::RED);
}

void PedestrianLight::SetState(PedestrianLightState state)
{
    current_state_ = state;
    blink_state_ = false;
    UpdateLights();
}

void PedestrianLight::UpdateBlink()
{
    if (current_state_ == PedestrianLightState::BLINKING)
    {
        blink_state_ = !blink_state_;
        UpdateLights();
    }
}

void PedestrianLight::TurnOffAll()
{
    left_red_.SetState(false);
    left_green_.SetState(false);
    right_red_.SetState(false);
    right_green_.SetState(false);
}

void PedestrianLight::UpdateLights()
{
    left_red_.SetState(false);
    left_green_.SetState(false);
    right_red_.SetState(false);
    right_green_.SetState(false);
    
    switch (current_state_)
    {
        case PedestrianLightState::RED:
            left_red_.SetState(true);
            right_red_.SetState(true);
            break;
            
        case PedestrianLightState::GREEN:
            left_green_.SetState(true);
            right_green_.SetState(true);
            break;
            
        case PedestrianLightState::BLINKING:
            if (blink_state_)
            {
                left_green_.SetState(true);
                right_green_.SetState(true);
            }
            break;
    }
}
