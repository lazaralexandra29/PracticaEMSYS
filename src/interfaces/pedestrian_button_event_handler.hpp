#ifndef INTERFACES_PEDESTRIAN_BUTTON_EVENT_HANDLER_HPP
#define INTERFACES_PEDESTRIAN_BUTTON_EVENT_HANDLER_HPP

class PedestrianButtonEventHandler
{
public:
    virtual ~PedestrianButtonEventHandler() = default;

    virtual void OnRightButtonInterrupt() = 0;
    virtual void OnLeftButtonInterrupt() = 0;
    virtual void OnSequenceTick() = 0;
    virtual void OnNightToggle() = 0;
};

#endif


