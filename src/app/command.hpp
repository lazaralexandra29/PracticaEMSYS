#ifndef COMMAND_HPP
#define COMMAND_HPP

class PedestrianButton;

class Command
{
public:
    explicit Command(PedestrianButton* pedestrian_button);

    void ExecuteCommand(const char* command);

private:
    PedestrianButton* pedestrian_button_;
};

#endif







