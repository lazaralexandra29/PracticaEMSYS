#define UNIT_TEST
#include "config/pin_config.hpp"
#include "drivers/usart_driver.hpp"
#include "drivers/timer_driver.hpp"
#include "app/adc.hpp"
#include "app/button.hpp"
#include "app/command.hpp"
#include "app/traffic_light.hpp"
#include "app/pedestrian_light.hpp"
#include "app/buzzer.hpp"
#include "app/light_sensor.hpp"
#include "app/pedestrian_button.hpp"
#include "app/pedestrian_button_router.hpp"
#include "app/uart_interface.hpp"
#include "app/logger.hpp"
#include <avr/io.h>
#include <stdint.h>

TimerDriver timerDriver;

// Single logger instance for dependency injection
Logger* logger = Logger::GetInstance();

// Inject logger into traffic lights to avoid multiple logger instances
TrafficLight trafficLights(
    TRAFFIC_LIGHT_LEFT_RED_PORT, TRAFFIC_LIGHT_LEFT_RED_PIN,
    TRAFFIC_LIGHT_LEFT_YELLOW_PORT, TRAFFIC_LIGHT_LEFT_YELLOW_PIN,
    TRAFFIC_LIGHT_LEFT_GREEN_PORT, TRAFFIC_LIGHT_LEFT_GREEN_PIN,
    TRAFFIC_LIGHT_RIGHT_RED_PORT, TRAFFIC_LIGHT_RIGHT_RED_PIN,
    TRAFFIC_LIGHT_RIGHT_YELLOW_PORT, TRAFFIC_LIGHT_RIGHT_YELLOW_PIN,
    TRAFFIC_LIGHT_RIGHT_GREEN_PORT, TRAFFIC_LIGHT_RIGHT_GREEN_PIN,
    logger
);


Button buttonRight(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
Button buttonLeft(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);

// Inject logger into pedestrian lights to avoid multiple logger instances
PedestrianLight pedestrianLights(
    PEDESTRIAN_LEFT_RED_PORT, PEDESTRIAN_LEFT_RED_PIN,
    PEDESTRIAN_LEFT_GREEN_PORT, PEDESTRIAN_LEFT_GREEN_PIN,
    PEDESTRIAN_RIGHT_RED_PORT, PEDESTRIAN_RIGHT_RED_PIN,
    PEDESTRIAN_RIGHT_GREEN_PORT, PEDESTRIAN_RIGHT_GREEN_PIN,
    logger
);

Buzzer buzzers(BUZZER_PORT, BUZZER_PIN);
HardwareUartInterface hardwareUart;

PedestrianButton pedestrianButton(
    &buttonRight,
    &buttonLeft,
    &trafficLights,
    &pedestrianLights,
    &buzzers,
    &timerDriver,
    &hardwareUart
);

Command commandManager(&pedestrianButton);

char commandBuffer[64];
bool wasDark = false;
uint32_t sensorCheckCounter = 0;

int main()
{
    // Initialize UART driver first (required for Logger)
    UsartStatus uart_status = UsartDriver::Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);
    if (!uart_status.IsSuccess())
    {
        while (1)
        {
            // Block on UART initialization failure
        }
    }

    hardwareUart.Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);

    trafficLights.Init();
    trafficLights.SetState(TrafficLightState::GREEN);

    buttonRight.Init();
    buttonLeft.Init();
    pedestrianLights.Init();
    buzzers.Init();
    pedestrianButton.Init();

    // Inject logger into ADC, LED, and TrafficLight static methods
    Adc::Init(logger);
    Led::SetLogger(logger);
    TrafficLight::SetLogger(logger);
    LightSensor::Init();

    logger->LogInfo("System initialized. Traffic lights synchronized. Two buttons configured (INT0=right, INT1=left).");
    trafficLights.ReportState();

    TimerConfiguration timerConfig(TimerMode::CTC, Prescaler::DIV_64);
    if (!timerDriver.InitTimer1(timerConfig).IsSuccess())
    {
        while (1)
        {
            // Block on timer initialization failure
        }
    }

    wasDark = LightSensor::IsDark();
    if (wasDark)
    {
        logger->LogInfo("[LIGHT_SENSOR] Status: NIGHT");
        pedestrianButton.HandleNightMode();
    }
    else
    {
        logger->LogInfo("[LIGHT_SENSOR] Status: DAY");
        pedestrianButton.HandleDayMode();
    }

    while (1)
    {
        timerDriver.Run();
        buttonRight.Update();
        buttonLeft.Update();
        pedestrianButton.Update();

        sensorCheckCounter++;
        if (sensorCheckCounter >= 200000)
        {
            sensorCheckCounter = 0;
            bool isDark = LightSensor::IsDark();

            if (isDark != wasDark)
            {
                wasDark = isDark;
                if (isDark)
                {
                    logger->LogInfo("[LIGHT_SENSOR] Status: NIGHT -> applying night mode");
                    pedestrianButton.HandleNightMode();
                }
                else
                {
                    logger->LogInfo("[LIGHT_SENSOR] Status: DAY -> restoring normal mode");
                    pedestrianButton.HandleDayMode();
                }
            }
        }

        if (hardwareUart.ReceiveLine(commandBuffer, sizeof(commandBuffer)))
        {
            commandManager.ExecuteCommand(commandBuffer);
        }
    }

    return 0;
}
