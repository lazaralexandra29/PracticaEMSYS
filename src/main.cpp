#include "config/pin_config.hpp"
#include "drivers/usart_driver.hpp"
#include "drivers/timer_driver.hpp"
#include "app/led.hpp"
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
#include <avr/io.h>
#include <stdint.h>

TimerDriver timerDriver;

TrafficLight trafficLights(
    TRAFFIC_LIGHT_LEFT_RED_PORT, TRAFFIC_LIGHT_LEFT_RED_PIN,
    TRAFFIC_LIGHT_LEFT_YELLOW_PORT, TRAFFIC_LIGHT_LEFT_YELLOW_PIN,
    TRAFFIC_LIGHT_LEFT_GREEN_PORT, TRAFFIC_LIGHT_LEFT_GREEN_PIN,
    TRAFFIC_LIGHT_RIGHT_RED_PORT, TRAFFIC_LIGHT_RIGHT_RED_PIN,
    TRAFFIC_LIGHT_RIGHT_YELLOW_PORT, TRAFFIC_LIGHT_RIGHT_YELLOW_PIN,
    TRAFFIC_LIGHT_RIGHT_GREEN_PORT, TRAFFIC_LIGHT_RIGHT_GREEN_PIN
);

Led greenLed(LED_GREEN_PORT, LED_GREEN_PIN);
Led redLed(LED_RED_PORT, LED_RED_PIN);
Led yellowLed(LED_YELLOW_PORT, LED_YELLOW_PIN);

Button buttonRight(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
Button buttonLeft(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);

PedestrianLight pedestrianLights(
    PEDESTRIAN_LEFT_RED_PORT, PEDESTRIAN_LEFT_RED_PIN,
    PEDESTRIAN_LEFT_GREEN_PORT, PEDESTRIAN_LEFT_GREEN_PIN,
    PEDESTRIAN_RIGHT_RED_PORT, PEDESTRIAN_RIGHT_RED_PIN,
    PEDESTRIAN_RIGHT_GREEN_PORT, PEDESTRIAN_RIGHT_GREEN_PIN
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
    hardwareUart.Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);

    trafficLights.Init();
    trafficLights.SetState(TrafficLightState::GREEN);

    greenLed.Init();
    redLed.Init();
    yellowLed.Init();
    redLed.RegisterInstance(1);
    yellowLed.RegisterInstance(2);

    buttonRight.Init();
    buttonLeft.Init();

    pedestrianLights.Init();
    buzzers.Init();
    pedestrianButton.Init();

    Adc::Init();
    LightSensor::Init();

    UsartDriver::send("System initialized. Traffic lights synchronized. Two buttons configured (INT0=right, INT1=left).\r\n");
    trafficLights.ReportState();

    TimerConfiguration timerConfig(TimerMode::CTC, Prescaler::DIV_64);
    if (!timerDriver.InitTimer1(timerConfig).IsSuccess())
    {
        while(1) {}
    }

    wasDark = LightSensor::IsDark();
    if (wasDark)
    {
        UsartDriver::send("[LIGHT_SENSOR] Status: NIGHT\r\n");
        pedestrianButton.HandleNightMode();
    }
    else
    {
        UsartDriver::send("[LIGHT_SENSOR] Status: DAY\r\n");
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
                    UsartDriver::send("[LIGHT_SENSOR] Status: NIGHT -> applying night mode\r\n");
                    pedestrianButton.HandleNightMode();
                }
                else
                {
                    UsartDriver::send("[LIGHT_SENSOR] Status: DAY -> restoring normal mode\r\n");
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