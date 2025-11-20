#ifndef TRAFFIC_SYSTEM_MANAGER_HPP
#define TRAFFIC_SYSTEM_MANAGER_HPP

#include "app/traffic_light.hpp"
#include "app/button.hpp"
#include "app/pedestrian_light.hpp"
#include "app/buzzer.hpp"
#include "app/pedestrian_button.hpp"
#include "app/command.hpp"
#include "app/uart_interface.hpp"
#include "app/light_sensor.hpp"
#include "app/adc.hpp"
#include "app/logger.hpp"
#include "app/ultrasonic_sensor.hpp"
#include "drivers/timer_driver.hpp"
#include "drivers/usart_driver.hpp"
#include <stdint.h>

class TrafficSystemManager
{
public:
    TrafficSystemManager(
        TimerDriver* timer_driver,
        TrafficLight* traffic_lights,
        Button* button_right,
        Button* button_left,
        PedestrianLight* pedestrian_lights,
        Buzzer* buzzers,
        PedestrianButton* pedestrian_button,
        Command* command_manager,
        IUartInterface* uart_interface,
        UltrasonicSensor* ultrasonic_sensor = nullptr
    );

    static TrafficSystemManager* GetInstance();
    static void SetInstance(TrafficSystemManager* instance);

    bool Initialize();
    
    void Update();
    
    TrafficLight* GetTrafficLights() const 
    { 
        return traffic_lights_; 
    }
    Button* GetButtonRight() const
    {
        return button_right_; 
    }
    Button* GetButtonLeft() const 
    { 
        return button_left_; 
    }
    PedestrianLight* GetPedestrianLights() const 
    { 
        return pedestrian_lights_; 
    }
    Buzzer* GetBuzzers() const 
    { 
        return buzzers_; 
    }
    PedestrianButton* GetPedestrianButton() const 
    { 
        return pedestrian_button_; 
    }
    Command* GetCommandManager() const 
    { 
        return command_manager_; 
    }
    
    void Log(LogLevel level, const char* message) const;

private:
    static TrafficSystemManager* instance_;

    TimerDriver* timer_driver_;
    TrafficLight* traffic_lights_;
    Button* button_right_;
    Button* button_left_;
    PedestrianLight* pedestrian_lights_;
    Buzzer* buzzers_;
    PedestrianButton* pedestrian_button_;
    Command* command_manager_;
    IUartInterface* uart_interface_;
    UltrasonicSensor* ultrasonic_sensor_;

    bool initialized_;
    bool was_dark_;
    uint32_t sensor_check_counter_;
    char command_buffer_[64];

    void InitializeComponents();
    void CheckLightSensor();
    void ProcessCommands();
    const char* GetLogLevelPrefix(LogLevel level) const;
    void ReportInitialUltrasonicDistance();
};

#endif


