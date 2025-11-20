#ifndef APP_PEDESTRIAN_BUTTON_ROUTER_HPP
#define APP_PEDESTRIAN_BUTTON_ROUTER_HPP

#include "interfaces/pedestrian_button_event_handler.hpp"

class PedestrianButtonEventRouter
{
public:
    static void RegisterHandler(PedestrianButtonEventHandler* handler);

    static void HandleRightButtonInterrupt();
    static void HandleLeftButtonInterrupt();
    static void HandleSequenceTick();
    static void HandleNightToggle();
};

#endif








