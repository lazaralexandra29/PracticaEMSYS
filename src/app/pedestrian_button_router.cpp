#include "app/pedestrian_button_router.hpp"

namespace
{
PedestrianButtonEventHandler* g_pedestrian_button_handler = nullptr;
}

void PedestrianButtonEventRouter::RegisterHandler(PedestrianButtonEventHandler* handler)
{
    g_pedestrian_button_handler = handler;
}

void PedestrianButtonEventRouter::HandleRightButtonInterrupt()
{
    if (g_pedestrian_button_handler != nullptr)
    {
        g_pedestrian_button_handler->OnRightButtonInterrupt();
    }
}

void PedestrianButtonEventRouter::HandleLeftButtonInterrupt()
{
    if (g_pedestrian_button_handler != nullptr)
    {
        g_pedestrian_button_handler->OnLeftButtonInterrupt();
    }
}

void PedestrianButtonEventRouter::HandleSequenceTick()
{
    if (g_pedestrian_button_handler != nullptr)
    {
        g_pedestrian_button_handler->OnSequenceTick();
    }
}

void PedestrianButtonEventRouter::HandleNightToggle()
{
    if (g_pedestrian_button_handler != nullptr)
    {
        g_pedestrian_button_handler->OnNightToggle();
    }
}








