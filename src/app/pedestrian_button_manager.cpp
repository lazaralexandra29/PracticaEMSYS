#include "app/pedestrian_button_manager.hpp"
#include "drivers/usart_driver.hpp"
#include <avr/interrupt.h>

static PedestrianButtonManager* instance = nullptr;

PedestrianButtonManager::PedestrianButtonManager(
    ButtonManager* buttonRight,
    ButtonManager* buttonLeft,
    TrafficLightManager* trafficLights,
    PedestrianLightManager* pedestrianLights,
    BuzzerManager* buzzers,
    TimerDriver* timerDriver)
    :
    buttonRight(buttonRight),
    buttonLeft(buttonLeft),
    trafficLights(trafficLights),
    pedestrianLights(pedestrianLights),
    buzzers(buzzers),
    timerDriver(timerDriver),
    sequenceState(PedestrianSequenceState::IDLE),
    buttonsEnabled(true),
    rightPressedFlag(false),
    leftPressedFlag(false),
    phaseTicks(0),
    tickTimerId(0xFF),
    nightToggleTimerId(0xFF)
{
}

void PedestrianButtonManager::init()
{
    instance = this;

    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);

    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC10);

    EIMSK |= (1 << INT0);
    EIMSK |= (1 << INT1);

    sei();

    tickTimerId = timerDriver->CreateTimerSoftware();
    if (tickTimerId < 8) 
    {
        timerDriver->RegisterPeriodicCallback(tickTimerId, onSequenceTick, tickMs);
    }
}

void PedestrianButtonManager::setButtonsEnabled(bool e)
{
    buttonsEnabled = e;
    if (e)
    {
        EIMSK |= (1 << INT0) | (1 << INT1);
    }
    else
    {
        EIMSK &= ~((1 << INT0) | (1 << INT1));
    }
}

void PedestrianButtonManager::handleInterruptRight()
{
    if (instance) instance->rightPressedFlag = true;
}

void PedestrianButtonManager::handleInterruptLeft()
{
    if (instance) instance->leftPressedFlag = true;
}

void PedestrianButtonManager::onSequenceTick()
{
    if (instance) instance->sequenceTick();
}

void PedestrianButtonManager::onNightToggle()
{
    if (instance && instance->trafficLights)
    {
        instance->trafficLights->toggleYellowLights();
    }
}

void PedestrianButtonManager::update()
{
    if (!buttonsEnabled) return;

    if (rightPressedFlag)
    {
        rightPressedFlag = false;
        if (sequenceState == PedestrianSequenceState::IDLE)
            startSequence();
    }

    if (leftPressedFlag)
    {
        leftPressedFlag = false;
        if (sequenceState == PedestrianSequenceState::IDLE)
            startSequence();
    }
}

void PedestrianButtonManager::startSequence()
{
    trafficLights->setState(TrafficLightState::GREEN);
    pedestrianLights->setState(PedestrianLightState::RED);
    buzzers->setState(false);

    enterState(PedestrianSequenceState::WAIT_GREEN, 3000);
}

void PedestrianButtonManager::enterState(PedestrianSequenceState st, uint32_t durationMs)
{
    sequenceState = st;
    phaseTicks = (durationMs + tickMs - 1) / tickMs;

    switch (st)
    {
        case PedestrianSequenceState::WAIT_GREEN:
            trafficLights->setState(TrafficLightState::GREEN);
            pedestrianLights->setState(PedestrianLightState::RED);
            buzzers->setState(false);
            break;

        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            trafficLights->setState(TrafficLightState::YELLOW);
            pedestrianLights->setState(PedestrianLightState::RED);
            buzzers->setState(false);
            break;

        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            trafficLights->setState(TrafficLightState::RED);
            pedestrianLights->setState(PedestrianLightState::GREEN);
            buzzers->setState(true);
            break;

        case PedestrianSequenceState::PEDESTRIAN_BLINK:
            pedestrianLights->setState(PedestrianLightState::BLINKING);
            break;

        default: break;
    }
}

void PedestrianButtonManager::sequenceTick()
{
    if (sequenceState == PedestrianSequenceState::IDLE)
        return;

    if (phaseTicks > 0)
        phaseTicks--;

    switch (sequenceState)
    {
        case PedestrianSequenceState::WAIT_GREEN:
            if (phaseTicks == 0)
                enterState(PedestrianSequenceState::TRANSITION_TO_YELLOW, 3000);
            break;

        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            if (phaseTicks == 0)
                enterState(PedestrianSequenceState::PEDESTRIAN_CROSSING, 7000);
            break;

        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            buzzers->toggle();
            if (phaseTicks == 0)
                enterState(PedestrianSequenceState::PEDESTRIAN_BLINK, 2000);
            break;

        case PedestrianSequenceState::PEDESTRIAN_BLINK:
            pedestrianLights->updateBlink();
            buzzers->toggle(); 
            if (phaseTicks == 0)
            {
                sequenceState = PedestrianSequenceState::IDLE;
                trafficLights->setState(TrafficLightState::GREEN);
                pedestrianLights->setState(PedestrianLightState::RED);
                buzzers->setState(false); 
            }
            break;

        default: break;
    }
}

void PedestrianButtonManager::handleNight()
{
    if (instance)
    {
        // Oprește orice secvență activă
        instance->sequenceState = PedestrianSequenceState::IDLE;
        instance->phaseTicks = 0;
        
        // Oprește buzzer-ul
        instance->buzzers->setState(false);
        
        // Stinge toate LED-urile de la pietoni
        instance->pedestrianLights->turnOffAll();
        
        // Setează semafoarele pe YELLOW (aprinde galben, stinge roșu și verde)
        instance->trafficLights->setState(TrafficLightState::YELLOW);
        
        // Oprește timer-ul night mode dacă există deja
        if (instance->nightToggleTimerId < 8)
        {
            instance->timerDriver->UnregisterPeriodicCallback(instance->nightToggleTimerId);
        }
        
        // Creează și înregistrează timer pentru toggle LED-uri galbene
        instance->nightToggleTimerId = instance->timerDriver->CreateTimerSoftware();
        if (instance->nightToggleTimerId < 8)
        {
            instance->timerDriver->RegisterPeriodicCallback(
                instance->nightToggleTimerId, 
                onNightToggle, 
                nightTogglePeriodMs
            );
        }
        
        UsartDriver::send("[NIGHT] Night mode activated: Yellow lights blinking (500ms), all other lights OFF.\r\n");
    }
    else
    {
        UsartDriver::send("ERR: PedestrianButtonManager not initialized.\r\n");
    }
}

void PedestrianButtonManager::handleDay()
{
    if (instance)
    {
        // Oprește timer-ul night mode
        if (instance->nightToggleTimerId < 8)
        {
            instance->timerDriver->UnregisterPeriodicCallback(instance->nightToggleTimerId);
            instance->nightToggleTimerId = 0xFF;
        }
        
        // Restabilește starea normală: verde pentru mașini, roșu pentru pietoni
        instance->sequenceState = PedestrianSequenceState::IDLE;
        instance->phaseTicks = 0;
        
        instance->trafficLights->setState(TrafficLightState::GREEN);
        instance->pedestrianLights->setState(PedestrianLightState::RED);
        instance->buzzers->setState(false);
        
        // Reactivează butoanele
        instance->setButtonsEnabled(true);
        
        UsartDriver::send("[DAY] Night mode deactivated. Normal operation restored (green for cars, red for pedestrians).\r\n");
    }
    else
    {
        UsartDriver::send("ERR: PedestrianButtonManager not initialized.\r\n");
    }
}

ISR(INT0_vect)
{
    PedestrianButtonManager::handleInterruptRight();
}

ISR(INT1_vect)
{
    PedestrianButtonManager::handleInterruptLeft();
}