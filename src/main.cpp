#include "config/pin_config.hpp"
#include "drivers/usart_driver.hpp"
#include "drivers/timer_driver.hpp"
#include "app/led_manager.hpp"
#include "app/adc_manager.hpp"
#include "app/button_manager.hpp"
#include "app/command_manager.hpp"
#include "app/traffic_light_manager.hpp"
#include "app/pedestrian_button_manager.hpp"
#include "app/pedestrian_light_manager.hpp"
#include "app/buzzer_manager.hpp"
#include "app/light_sensor_manager.hpp"
#include <avr/io.h>
#include <stdint.h>

TimerDriver timerDriver;

TrafficLightManager trafficLights(
    TRAFFIC_LIGHT_LEFT_RED_PORT, TRAFFIC_LIGHT_LEFT_RED_PIN,
    TRAFFIC_LIGHT_LEFT_YELLOW_PORT, TRAFFIC_LIGHT_LEFT_YELLOW_PIN,
    TRAFFIC_LIGHT_LEFT_GREEN_PORT, TRAFFIC_LIGHT_LEFT_GREEN_PIN,
    TRAFFIC_LIGHT_RIGHT_RED_PORT, TRAFFIC_LIGHT_RIGHT_RED_PIN,
    TRAFFIC_LIGHT_RIGHT_YELLOW_PORT, TRAFFIC_LIGHT_RIGHT_YELLOW_PIN,
    TRAFFIC_LIGHT_RIGHT_GREEN_PORT, TRAFFIC_LIGHT_RIGHT_GREEN_PIN
);

LedManager greenLed(LED_GREEN_PORT, LED_GREEN_PIN);
LedManager redLed(LED_RED_PORT, LED_RED_PIN);
LedManager yellowLed(LED_YELLOW_PORT, LED_YELLOW_PIN);

ButtonManager buttonRight(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
ButtonManager buttonLeft(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);

PedestrianLightManager pedestrianLights(
    PEDESTRIAN_LEFT_RED_PORT, PEDESTRIAN_LEFT_RED_PIN,
    PEDESTRIAN_LEFT_GREEN_PORT, PEDESTRIAN_LEFT_GREEN_PIN,
    PEDESTRIAN_RIGHT_RED_PORT, PEDESTRIAN_RIGHT_RED_PIN,
    PEDESTRIAN_RIGHT_GREEN_PORT, PEDESTRIAN_RIGHT_GREEN_PIN
);

BuzzerManager buzzers(
    BUZZER_LEFT_PORT, BUZZER_LEFT_PIN,
    BUZZER_RIGHT_PORT, BUZZER_RIGHT_PIN
);

PedestrianButtonManager pedestrianButtonManager(
    &buttonRight,
    &buttonLeft,
    &trafficLights,
    &pedestrianLights,
    &buzzers,
    &timerDriver
);

char commandBuffer[64];
bool wasDark = false;
uint32_t sensorCheckCounter = 0;

int main()
{
    UsartDriver::Init(UsartBaudRate::BR9600, UsartParity::NONE, UsartStopBits::ONE);

    trafficLights.init();
    trafficLights.setState(TrafficLightState::GREEN);

    greenLed.init();
    redLed.init();
    yellowLed.init();
    redLed.registerInstance(1);
    yellowLed.registerInstance(2);

    buttonRight.init();
    buttonLeft.init();

    pedestrianLights.init();
    buzzers.init();
    pedestrianButtonManager.init();

    AdcManager::init();
    LightSensorManager::init();

    UsartDriver::send("System initialized. Traffic lights synchronized. Two buttons configured (INT0=right, INT1=left).\r\n");
    trafficLights.reportState();

    TimerConfiguration timerConfig(TimerMode::CTC, Prescaler::DIV_64);
    if (!timerDriver.InitTimer1(timerConfig).IsSuccess())
    {
        while(1) {}
    }

    wasDark = LightSensorManager::isDark();
    if (wasDark)
    {
        UsartDriver::send("[LIGHT_SENSOR] Status: NIGHT\r\n");
        PedestrianButtonManager::handleNight();
    }
    else
    {
        UsartDriver::send("[LIGHT_SENSOR] Status: DAY\r\n");
        PedestrianButtonManager::handleDay();
    }

    while (1)
    {
        timerDriver.Run();

        buttonRight.update();
        buttonLeft.update();
        pedestrianButtonManager.update();

        sensorCheckCounter++;
        if (sensorCheckCounter >= 200000) // verifică mai des (sensibilitate sporită)
        {
            sensorCheckCounter = 0;
            bool isDark = LightSensorManager::isDark();
            if (isDark != wasDark)
            {
                wasDark = isDark;
                if (isDark)
                {
                    UsartDriver::send("[LIGHT_SENSOR] Status: NIGHT -> applying night mode\r\n");
                    PedestrianButtonManager::handleNight();
                }
                else
                {
                    UsartDriver::send("[LIGHT_SENSOR] Status: DAY -> restoring normal mode\r\n");
                    PedestrianButtonManager::handleDay();
                }
            }
        }

        if (UsartDriver::receiveLineNonBlocking(commandBuffer, sizeof(commandBuffer)))
        {
            CommandManager::executeCommand(commandBuffer);
        }
    }

    return 0;
}