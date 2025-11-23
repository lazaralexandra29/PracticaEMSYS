#include "app/traffic_system_manager.hpp"
#include "app/hardware_uart_interface.hpp"
#include "app/logger.hpp"
#include "drivers/usart_driver.hpp"
#include "config/pin_config.hpp"
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

TrafficSystemManager* TrafficSystemManager::instance_ = nullptr;

TrafficSystemManager::TrafficSystemManager(
    TimerDriver* timer_driver,
    TrafficLight* traffic_lights,
    Button* button_right,
    Button* button_left,
    PedestrianLight* pedestrian_lights,
    Buzzer* buzzers,
    PedestrianButton* pedestrian_button,
    Command* command_manager,
    IUartInterface* uart_interface,
    ILogger* logger,
    UltrasonicSensor* ultrasonic_sensor
)
    : timer_driver_(timer_driver),
      traffic_lights_(traffic_lights),
      button_right_(button_right),
      button_left_(button_left),
      pedestrian_lights_(pedestrian_lights),
      buzzers_(buzzers),
      pedestrian_button_(pedestrian_button),
      command_manager_(command_manager),
      uart_interface_(uart_interface),
      logger_(logger != nullptr ? logger : Logger::GetInstance()),
      ultrasonic_sensor_(ultrasonic_sensor),
      initialized_(false),
      was_dark_(false),
      sensor_check_counter_(0)
{
    command_buffer_[0] = '\0';
}

TrafficSystemManager* TrafficSystemManager::GetInstance()
{
    return instance_;
}

void TrafficSystemManager::SetInstance(TrafficSystemManager* instance)
{
    instance_ = instance;
}

bool TrafficSystemManager::Initialize()
{
    if (initialized_)
    {
        logger_->Log(LogLevel::WARNING, "System already initialized.");
        return false;
    }

    if (timer_driver_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "TimerDriver dependency is NULL!");
        return false;
    }

    if (traffic_lights_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "TrafficLight dependency is NULL!");
        return false;
    }

    if (button_right_ == nullptr || button_left_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "Button dependency is NULL!");
        return false;
    }

    if (pedestrian_lights_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "PedestrianLight dependency is NULL!");
        return false;
    }

    if (buzzers_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "Buzzer dependency is NULL!");
        return false;
    }

    if (pedestrian_button_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "PedestrianButton dependency is NULL!");
        return false;
    }

    if (command_manager_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "Command dependency is NULL!");
        return false;
    }

    if (uart_interface_ == nullptr)
    {
        logger_->Log(LogLevel::ERROR, "UartInterface dependency is NULL!");
        return false;
    }

    UsartStatus uart_status = UsartDriver::Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);
    if (!uart_status.IsSuccess())
    {
        logger_->Log(LogLevel::ERROR, "UART initialization failed!");
        return false;
    }

    TimerConfiguration timerConfig(TimerMode::CTC, Prescaler::DIV_64);
    if (!timer_driver_->InitTimer1(timerConfig).IsSuccess())
    {
        logger_->Log(LogLevel::ERROR, "Timer initialization failed!");
        return false;
    }

    InitializeComponents();

    // Note: Adc and LightSensor initialization should be done in main.cpp
    // as they require global instances

    HardwareUartInterface* hardware_uart = static_cast<HardwareUartInterface*>(uart_interface_);
    if (hardware_uart)
    {
        hardware_uart->Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);
    }

    was_dark_ = LightSensor::IsDark();
    if (was_dark_)
    {
        pedestrian_button_->HandleNightMode();
    }
    else
    {
        pedestrian_button_->HandleDayMode();
    }

    ReportInitialUltrasonicDistance();

    initialized_ = true;
    return true;
}

void TrafficSystemManager::InitializeComponents()
{
    if (traffic_lights_)
    {
        traffic_lights_->Init();
        traffic_lights_->SetState(TrafficLightState::GREEN);
    }

    if (button_right_)
    {
        button_right_->Init();
    }

    if (button_left_)
    {
        button_left_->Init();
    }

    if (pedestrian_lights_)
    {
        pedestrian_lights_->Init();
    }

    if (buzzers_)
    {
        buzzers_->Init();
    }

    if (pedestrian_button_)
    {
        pedestrian_button_->Init();
    }

    if (ultrasonic_sensor_)
    {
        ultrasonic_sensor_->Init();
    }
}

void TrafficSystemManager::Update()
{
    if (!initialized_)
    {
        return;
    }

    if (timer_driver_)
    {
        timer_driver_->Run();
    }

    if (button_right_)
    {
        button_right_->Update();
    }

    if (button_left_)
    {
        button_left_->Update();
    }

    if (pedestrian_button_)
    {
        pedestrian_button_->Update();
    }

    CheckLightSensor();

    ProcessCommands();
}

void TrafficSystemManager::CheckLightSensor()
{
    sensor_check_counter_++;
    if (sensor_check_counter_ >= 200000) // Check every ~10 seconds
    {
        sensor_check_counter_ = 0;
        
        bool is_dark = LightSensor::IsDark();
        if (is_dark != was_dark_)
        {
            was_dark_ = is_dark;
            if (is_dark)
            {
                UsartDriver::send("[LIGHT_SENSOR] Status: NIGHT -> applying night mode\r\n");
                if (pedestrian_button_)
                {
                    pedestrian_button_->HandleNightMode();
                }
            }
            else
            {
                UsartDriver::send("[LIGHT_SENSOR] Status: DAY -> restoring normal mode\r\n");
                if (pedestrian_button_)
                {
                    pedestrian_button_->HandleDayMode();
                }
            }
        }
    }
}

void TrafficSystemManager::ProcessCommands()
{
    if (uart_interface_ && command_manager_)
    {
        if (uart_interface_->ReceiveLine(command_buffer_, sizeof(command_buffer_)))
        {
            command_manager_->ExecuteCommand(command_buffer_);
        }
    }
}


void TrafficSystemManager::ReportInitialUltrasonicDistance()
{
    if (ultrasonic_sensor_ == nullptr)
    {
        return;
    }

    uint16_t distance_cm = 0;
    if (ultrasonic_sensor_->MeasureDistanceCm(distance_cm))
    {
        char buffer[96];
        snprintf(buffer, sizeof(buffer), "[ULTRASONIC] Distanta initiala: %u cm\r\n", distance_cm);
        UsartDriver::send(buffer);
    }
    else
    {
        UsartDriver::send("[ULTRASONIC] Nu pot citi distanta (timeout)\r\n");
    }
}

