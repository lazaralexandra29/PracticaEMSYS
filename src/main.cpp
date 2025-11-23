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
#include "app/hardware_uart_interface.hpp"
#include "app/logger.hpp"
#include "app/ultrasonic_sensor.hpp"
#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>

namespace
{
volatile bool g_ultrasonic_measure_request = false;
constexpr uint32_t kUltrasonicMeasurePeriodMs = 1000;

void UltrasonicMeasurementRequest()
{
    g_ultrasonic_measure_request = true;
}
}

TimerDriver timerDriver;

ILogger* logger = Logger::GetInstance();

TrafficLight trafficLights(
    TRAFFIC_LIGHT_LEFT_RED_PORT, TRAFFIC_LIGHT_LEFT_RED_PIN,
    TRAFFIC_LIGHT_LEFT_YELLOW_PORT, TRAFFIC_LIGHT_LEFT_YELLOW_PIN,
    TRAFFIC_LIGHT_LEFT_GREEN_PORT, TRAFFIC_LIGHT_LEFT_GREEN_PIN,
    TRAFFIC_LIGHT_RIGHT_RED_PORT, TRAFFIC_LIGHT_RIGHT_RED_PIN,
    TRAFFIC_LIGHT_RIGHT_YELLOW_PORT, TRAFFIC_LIGHT_RIGHT_YELLOW_PIN,
    TRAFFIC_LIGHT_RIGHT_GREEN_PORT, TRAFFIC_LIGHT_RIGHT_GREEN_PIN,
    *logger
);


Button buttonRight(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
Button buttonLeft(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);

PedestrianLight pedestrianLights(
    PEDESTRIAN_LEFT_RED_PORT, PEDESTRIAN_LEFT_RED_PIN,
    PEDESTRIAN_LEFT_GREEN_PORT, PEDESTRIAN_LEFT_GREEN_PIN,
    PEDESTRIAN_RIGHT_RED_PORT, PEDESTRIAN_RIGHT_RED_PIN,
    PEDESTRIAN_RIGHT_GREEN_PORT, PEDESTRIAN_RIGHT_GREEN_PIN,
    *logger
);

Buzzer buzzers(BUZZER_PORT, BUZZER_PIN);
HardwareUartInterface hardwareUart;
UltrasonicSensor ultrasonicSensor(
    ULTRASONIC_TRIG_PORT,
    ULTRASONIC_TRIG_PIN,
    ULTRASONIC_ECHO_PORT,
    ULTRASONIC_ECHO_PIN);

Adc adc(logger);

PedestrianButton pedestrianButton(
    &buttonRight,
    &buttonLeft,
    &trafficLights,
    &pedestrianLights,
    &buzzers,
    &timerDriver,
    &hardwareUart,
    logger
);

Command commandManager(&pedestrianButton);

char commandBuffer[64];
bool wasDark = false;
uint32_t sensorCheckCounter = 0;

uint16_t last_ultrasonic_distance_cm = 0;
bool has_ultrasonic_history = false;
bool buzzer_activated_by_speed = false; 
bool traffic_lights_red_for_speed = false; 
bool was_night_mode_before_speed = false;
TrafficLightState previous_traffic_state = TrafficLightState::GREEN; 
static constexpr int16_t kSpeedThresholdCmPerS = 11;
static constexpr uint16_t kMaxDistanceCm = 20; 

int main()
{
    UsartStatus uart_status = UsartDriver::Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);
    if (!uart_status.IsSuccess())
    {
        while (1)
        {

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
    ultrasonicSensor.Init();

    adc.Init();
    LightSensor::Init(&adc);

    TimerConfiguration timerConfig(TimerMode::CTC, Prescaler::DIV_64);
    if (!timerDriver.InitTimer1(timerConfig).IsSuccess())
    {
        while (1)
        {

        }
    }

    uint8_t ultrasonicTimerId = timerDriver.CreateTimerSoftware();
    if (ultrasonicTimerId < 8)
    {
        timerDriver.RegisterPeriodicCallback(ultrasonicTimerId, UltrasonicMeasurementRequest, kUltrasonicMeasurePeriodMs);
    }

    wasDark = LightSensor::IsDark();
    if (wasDark)
    {
        pedestrianButton.HandleNightMode();
    }
    else
    {
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
                    logger->Log(LogLevel::INFO, "[LIGHT_SENSOR] Status: NIGHT -> applying night mode");
                    pedestrianButton.HandleNightMode();
                }
                else
                {
                    logger->Log(LogLevel::INFO, "[LIGHT_SENSOR] Status: DAY -> restoring normal mode");
                    pedestrianButton.HandleDayMode();
                }
            }
        }

        if (hardwareUart.ReceiveLine(commandBuffer, sizeof(commandBuffer)))
        {
            commandManager.ExecuteCommand(commandBuffer);
        }

        if (g_ultrasonic_measure_request)
        {
            g_ultrasonic_measure_request = false;
            uint16_t distance_cm = 0;
            if (ultrasonicSensor.MeasureDistanceCm(distance_cm))
            {
                if (has_ultrasonic_history)
                {
                    int16_t delta_distance = static_cast<int16_t>(last_ultrasonic_distance_cm) - static_cast<int16_t>(distance_cm);
                    int16_t speed_cm_per_s = delta_distance;
                    bool object_approaching = (delta_distance > 0);
                    
                    if (!pedestrianButton.IsSequenceActive())
                    {
                        bool should_log = object_approaching && distance_cm <= kMaxDistanceCm;
                        if (should_log)
                        {
                            char buffer[80];
                            snprintf(buffer, sizeof(buffer), "[ALERT] Masina apropiindu-se! Viteza: %d cm/s, Distanta: %u cm\r\n",
                                    speed_cm_per_s, distance_cm);
                            UsartDriver::send(buffer);
                        }

                        bool speed_exceeds_threshold = speed_cm_per_s > kSpeedThresholdCmPerS;
                        if (speed_exceeds_threshold)
                        {
                            if (!buzzer_activated_by_speed)
                            {
                                previous_traffic_state = trafficLights.GetState();
                                
                                was_night_mode_before_speed = pedestrianButton.IsNightMode();
                                if (was_night_mode_before_speed)
                                {
                                    pedestrianButton.PauseNightToggle();
                                }
                                
                                trafficLights.SetState(TrafficLightState::RED);
                                traffic_lights_red_for_speed = true;
                            }
                            buzzers.SetState(true);
                            buzzer_activated_by_speed = true;
                        }
                        else
                        {
                            if (buzzer_activated_by_speed)
                            {
                                buzzers.SetState(false);
                                buzzer_activated_by_speed = false;
                                if (traffic_lights_red_for_speed)
                                {
                                    trafficLights.SetState(previous_traffic_state);
                                    traffic_lights_red_for_speed = false;
                                    
                                    if (was_night_mode_before_speed)
                                    {
                                        pedestrianButton.ResumeNightToggle();
                                        was_night_mode_before_speed = false;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        buzzer_activated_by_speed = false;
                    }
                }
                else
                {
                    has_ultrasonic_history = true;
                }
                
                last_ultrasonic_distance_cm = distance_cm;
            }
        }
    }

    return 0;
}