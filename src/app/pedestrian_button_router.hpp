#ifndef APP_PEDESTRIAN_BUTTON_ROUTER_HPP
#define APP_PEDESTRIAN_BUTTON_ROUTER_HPP

class PedestrianButtonEventHandler
{
public:
    virtual ~PedestrianButtonEventHandler() = default;

    virtual void OnRightButtonInterrupt() = 0;
    virtual void OnLeftButtonInterrupt() = 0;
    virtual void OnSequenceTick() = 0;
    virtual void OnNightToggle() = 0;
};

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



