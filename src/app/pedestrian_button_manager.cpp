#include "app/pedestrian_button_manager.hpp"
#include "drivers/usart_driver.hpp"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define MAX_SOFTWARE_TIMERS 8 

static PedestrianButtonManager* g_pedestrianManager = nullptr;

PedestrianButtonManager::PedestrianButtonManager(
    ButtonManager* buttonRight,
    ButtonManager* buttonLeft,
    TrafficLightManager* trafficLights,
    PedestrianLightManager* pedestrianLights,
    BuzzerManager* buzzers,
    TimerDriver* timerDriver)
    : buttonRight(buttonRight),
      buttonLeft(buttonLeft),
      trafficLights(trafficLights),
      pedestrianLights(pedestrianLights),
      buzzers(buzzers),
      timerDriver(timerDriver),
      pressCount(0),
      rightPressCount(0),
      leftPressCount(0),
      sequenceState(PedestrianSequenceState::IDLE),
      buttonRightPressedFlag(false),
      buttonLeftPressedFlag(false),
      buttonsEnabled(true),
      secondsRemaining(0),
      buzzerToggleTimerId(MAX_SOFTWARE_TIMERS),
      buzzerBipTimerId(MAX_SOFTWARE_TIMERS),
      pedestrianBlinkTimerId(MAX_SOFTWARE_TIMERS),
      yellowToggleTimerId(MAX_SOFTWARE_TIMERS),
      buzzerBipState(false),
      nightModeActive(false),
      nightToggleCounter(0)
{
}

void PedestrianButtonManager::init()
{
    sei();
    
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    
    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC10);
    
    EIMSK |= (1 << INT0);  
    EIMSK |= (1 << INT1);
    
    g_pedestrianManager = this;
    
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
        "[PEDESTRIAN] Button manager initialized. EICRA=0x%02X, EIMSK=0x%02X\r\n",
        EICRA, EIMSK);
    UsartDriver::send(buffer);
}

void PedestrianButtonManager::setButtonsEnabled(bool enabled)
{
    buttonsEnabled = enabled;
    
    if (enabled)
    {
        EIMSK |= (1 << INT0);  
        EIMSK |= (1 << INT1); 
    }
    else
    {
        EIMSK &= ~(1 << INT0); 
        EIMSK &= ~(1 << INT1);
    }
}

void PedestrianButtonManager::update()
{
    if (nightModeActive)
    {
        nightToggleCounter++;
        if (nightToggleCounter >= 50000)
        {
            nightToggleCounter = 0;
            if (trafficLights->getState() == TrafficLightState::YELLOW)
            {
                trafficLights->toggleYellowLights();
            }
        }
        return;
    }
    
    if (!buttonsEnabled)
    {
        return;
    }
    
    if (buttonRightPressedFlag)
    {
        buttonRightPressedFlag = false;
        
        UsartDriver::send("[PEDESTRIAN] Right button interrupt detected! Processing...\r\n");
        
        if (sequenceState == PedestrianSequenceState::IDLE)
        {
            pressCount++;
            rightPressCount++;
            logButtonPress("RIGHT");
            startSequence();
        }
        else
        {
            rightPressCount++;
            char buffer[128];
            snprintf(buffer, sizeof(buffer), 
                "[PEDESTRIAN] Right button pressed but sequence already in progress (state: %u). Ignoring (priority rule).\r\n",
                (uint8_t)sequenceState);
            UsartDriver::send(buffer);
        }
    }
    
    if (buttonLeftPressedFlag)
    {
        buttonLeftPressedFlag = false;
        
        UsartDriver::send("[PEDESTRIAN] Left button interrupt detected! Processing...\r\n");
        
        if (sequenceState == PedestrianSequenceState::IDLE)
        {
            pressCount++;
            leftPressCount++;
            logButtonPress("LEFT");
            startSequence();
        }
        else
        {
            leftPressCount++;
            char buffer[128];
            snprintf(buffer, sizeof(buffer), 
                "[PEDESTRIAN] Left button pressed but sequence already in progress (state: %u). Ignoring (priority rule).\r\n",
                (uint8_t)sequenceState);
            UsartDriver::send(buffer);
        }
    }
}

void PedestrianButtonManager::handleInterruptRight()
{
    if (g_pedestrianManager)
    {
        g_pedestrianManager->buttonRightPressedFlag = true;
    }
}

void PedestrianButtonManager::handleInterruptLeft()
{
    if (g_pedestrianManager)
    {
        g_pedestrianManager->buttonLeftPressedFlag = true;
    }
}

// Callback-uri pentru timer-uri - nu mai sunt folosite
void PedestrianButtonManager::onYellowTimeout() {}
void PedestrianButtonManager::onRedTimeout() {}
void PedestrianButtonManager::onPedestrianTimeout() {}

void PedestrianButtonManager::onBuzzerToggle()
{
    // Fara timer pentru toggle buzzer -> delay-uri blocante
}

void PedestrianButtonManager::onBuzzerBip()
{
    // Fara timer pentru bip buzzer -> folosim delay-uri blocante
}

void PedestrianButtonManager::onPedestrianBlink()
{
    // Fara timer pentru toggle LED-uri -> delay-uri blocante
}

void PedestrianButtonManager::onYellowToggle()
{
    if (g_pedestrianManager && g_pedestrianManager->trafficLights)
    {
        if (g_pedestrianManager->trafficLights->getState() == TrafficLightState::YELLOW)
        {
            g_pedestrianManager->trafficLights->toggleYellowLights();
        }
    }
}

void PedestrianButtonManager::startSequence()
{
    if (sequenceState != PedestrianSequenceState::IDLE)
    {
        UsartDriver::send("[PEDESTRIAN] Sequence already in progress. Ignoring button press.\r\n");
        return;
    }
    
    buzzers->setState(false);
    
    UsartDriver::send("[PEDESTRIAN] Button pressed! Starting sequence: GREEN (3s) -> YELLOW (3s) -> RED (7s for pedestrians) -> GREEN.\r\n");
    
    logDetailedStatus();
    
    sequenceState = PedestrianSequenceState::IDLE; 
    trafficLights->setState(TrafficLightState::GREEN);
    pedestrianLights->setState(PedestrianLightState::RED);
    buzzers->setState(false);
    secondsRemaining = 3;
    UsartDriver::send("[PEDESTRIAN] Keeping GREEN for 3 seconds...\r\n");
    logSequenceState();
    
    for (uint8_t i = 0; i < 30; i++) 
    {
        _delay_ms(100);
        secondsRemaining = 3 - (i / 10);
        
        if ((i % 10) == 0 && i > 0)
        {
            logDetailedStatus();
        }
    }
    
    sequenceState = PedestrianSequenceState::TRANSITION_TO_YELLOW;
    trafficLights->setState(TrafficLightState::YELLOW);
    pedestrianLights->setState(PedestrianLightState::RED);
    buzzers->setState(false);
    secondsRemaining = 3;
    UsartDriver::send("[PEDESTRIAN] Transition to YELLOW (3s remaining).\r\n");
    logSequenceState();
    
    for (uint8_t i = 0; i < 30; i++)  // 30 * 100ms = 3000ms
    {
        _delay_ms(100);
        secondsRemaining = 3 - (i / 10);
        
        if ((i % 10) == 0 && i > 0)
        {
            logDetailedStatus();
        }
    }
    
    sequenceState = PedestrianSequenceState::PEDESTRIAN_CROSSING;
    trafficLights->setState(TrafficLightState::RED);
    pedestrianLights->setState(PedestrianLightState::GREEN); 
    secondsRemaining = 7;
    UsartDriver::send("[PEDESTRIAN] RED - Pedestrians can now cross (7s remaining). Buzzers ON.\r\n");
    logSequenceState();
    
    for (uint8_t i = 0; i < 25; i++)
    {
        secondsRemaining = 7 - ((i * 2) / 20);
        
        if ((i % 2) == 0)
        {
            buzzers->setState(true);
            for (uint8_t j = 0; j < 10; j++)
            {
                _delay_ms(10);
                buzzers->update();
            }
        }
        else
        {
            buzzers->setState(false);
            _delay_ms(100);
        }
        
        if ((i % 10) == 0 && i > 0)
        {
            logDetailedStatus();
        }
    }
    
    pedestrianLights->setState(PedestrianLightState::BLINKING);
    UsartDriver::send("[PEDESTRIAN] Warning: Green blinking (2s remaining before cars get GREEN).\r\n");
    
    for (uint8_t i = 0; i < 10; i++)
    {
        secondsRemaining = 2 - (i / 5);
        
        if ((i % 2) == 0)
        {
            buzzers->setState(true);
            for (uint8_t j = 0; j < 10; j++)  // 10 * 10ms = 100ms
            {
                _delay_ms(10);
                buzzers->update();
            }
        }
        else
        {
            buzzers->setState(false);
            _delay_ms(100);
        }
        
        if ((i % 2) == 0)
        {
            pedestrianLights->updateBlink();
        }
        
        if ((i % 5) == 0 && i > 0)
        {
            logDetailedStatus();
        }
    }
    
    buzzers->setState(false);
    
    sequenceState = PedestrianSequenceState::IDLE;
    trafficLights->setState(TrafficLightState::GREEN);
    pedestrianLights->setState(PedestrianLightState::RED);
    buzzers->setState(false);
    secondsRemaining = 0;
    
    UsartDriver::send("[PEDESTRIAN] Sequence complete. Traffic lights back to GREEN.\r\n");
    logSequenceState();
}

void PedestrianButtonManager::stopAllTimers()
{
    // Oprește DOAR timer-ul pentru toggle LED-uri galbene (modul noapte)
    // NU oprește buzzerul - acesta este controlat de secvența normală
    if (yellowToggleTimerId < MAX_SOFTWARE_TIMERS)
    {
        timerDriver->UnregisterPeriodicCallback(yellowToggleTimerId);
        yellowToggleTimerId = MAX_SOFTWARE_TIMERS;
    }
}

void PedestrianButtonManager::startNightMode()
{
    stopAllTimers();
    
    pedestrianLights->turnOffAll();
    
    trafficLights->setState(TrafficLightState::YELLOW);
    
    // 3. Creează timer pentru toggle LED-uri galbene la 3ms
    yellowToggleTimerId = timerDriver->CreateTimerSoftware();
    if (yellowToggleTimerId < MAX_SOFTWARE_TIMERS)
    {
        timerDriver->RegisterPeriodicCallback(yellowToggleTimerId, onYellowToggle, 3);
    }
    
}

void PedestrianButtonManager::stopNightMode()
{
    stopAllTimers();
}

void PedestrianButtonManager::logButtonPress(const char* side)
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
        "[PEDESTRIAN] %s button pressed! Total: %u | Right: %u | Left: %u\r\n", 
        side, pressCount, rightPressCount, leftPressCount);
    UsartDriver::send(buffer);
}

void PedestrianButtonManager::logSequenceState()
{
    char buffer[256];
    const char* stateStr = "";
    
    switch (sequenceState)
    {
        case PedestrianSequenceState::IDLE:
            stateStr = "IDLE (GREEN for cars)";
            break;
        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            stateStr = "TRANSITION_TO_YELLOW (2s remaining)";
            break;
        case PedestrianSequenceState::TRANSITION_TO_RED:
            stateStr = "TRANSITION_TO_RED (2s remaining)";
            break;
        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            stateStr = "PEDESTRIAN_CROSSING";
            break;
    }
    
    snprintf(buffer, sizeof(buffer), 
        "[PEDESTRIAN] State: %s | Seconds remaining: %u | Button presses: %u\r\n",
        stateStr, secondsRemaining, pressCount);
    UsartDriver::send(buffer);
}

void PedestrianButtonManager::logTimeRemaining()
{
    if (sequenceState != PedestrianSequenceState::IDLE)
    {
        char buffer[256];
        if (arePedestriansCrossing())
        {
            snprintf(buffer, sizeof(buffer), 
                "[PEDESTRIAN] Status: Pedestrians crossing (RED) | Seconds until GREEN: %u | Button presses: %u\r\n",
                secondsRemaining, pressCount);
        }
        else if (sequenceState == PedestrianSequenceState::TRANSITION_TO_YELLOW)
        {
            snprintf(buffer, sizeof(buffer), 
                "[PEDESTRIAN] Status: Yellow phase | Seconds until RED: %u | Button presses: %u\r\n",
                secondsRemaining, pressCount);
        }
        else if (sequenceState == PedestrianSequenceState::TRANSITION_TO_RED)
        {
            snprintf(buffer, sizeof(buffer), 
                "[PEDESTRIAN] Status: Transitioning to RED | Seconds until pedestrian crossing: %u | Button presses: %u\r\n",
                secondsRemaining, pressCount);
        }
        UsartDriver::send(buffer);
    }
}

void PedestrianButtonManager::logButtonState()
{
    char buffer[256];
    buttonRight->update();
    buttonLeft->update();
    bool rightPressed = buttonRight->isPressed();
    bool leftPressed = buttonLeft->isPressed();
    
    snprintf(buffer, sizeof(buffer), 
        "[PEDESTRIAN] Right button: %s | Left button: %s | Total: %u | Right: %u | Left: %u\r\n",
        rightPressed ? "PRESSED" : "NOT PRESSED",
        leftPressed ? "PRESSED" : "NOT PRESSED",
        pressCount, rightPressCount, leftPressCount);
    UsartDriver::send(buffer);
}

void PedestrianButtonManager::logFullStatus()
{
    char buffer[320];
    buttonRight->update();
    buttonLeft->update();
    bool rightPressed = buttonRight->isPressed();
    bool leftPressed = buttonLeft->isPressed();
    bool pedestriansCrossing = arePedestriansCrossing();
    
    const char* stateStr = "";
    const char* trafficStateStr = "";
    uint8_t secondsToGreen = 0;
    
    switch (sequenceState)
    {
        case PedestrianSequenceState::IDLE:
            stateStr = "IDLE";
            trafficStateStr = "GREEN (for cars)";
            secondsToGreen = 0;
            break;
        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            stateStr = "YELLOW phase";
            trafficStateStr = "YELLOW";
            secondsToGreen = secondsRemaining + 4; // 2s yellow + 2s red transition + 5s pedestrians
            break;
        case PedestrianSequenceState::TRANSITION_TO_RED:
            stateStr = "RED transition";
            trafficStateStr = "RED";
            secondsToGreen = secondsRemaining + 5; // 2s red transition + 5s pedestrians
            break;
        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            stateStr = "PEDESTRIAN crossing";
            trafficStateStr = "RED (for pedestrians)";
            secondsToGreen = secondsRemaining;
            break;
    }
    
    snprintf(buffer, sizeof(buffer), 
        "[PEDESTRIAN] === FULL STATUS ===\r\n"
        "  Right button: %s | Left button: %s\r\n"
        "  Total presses: %u | Right: %u | Left: %u\r\n"
        "  Sequence: %s | Traffic: %s\r\n"
        "  Pedestrians crossing: %s\r\n"
        "  Seconds until GREEN: %u\r\n"
        "  Seconds in YELLOW phase: %u\r\n"
        "==========================\r\n",
        rightPressed ? "PRESSED" : "NOT PRESSED",
        leftPressed ? "PRESSED" : "NOT PRESSED",
        pressCount, rightPressCount, leftPressCount,
        stateStr,
        trafficStateStr,
        pedestriansCrossing ? "YES" : "NO",
        secondsToGreen,
        (sequenceState == PedestrianSequenceState::TRANSITION_TO_YELLOW) ? secondsRemaining : 0);
    UsartDriver::send(buffer);
}

void PedestrianButtonManager::logDetailedStatus()
{
    char buffer[320];
    PedestrianLightState pedState = pedestrianLights->getState();
    
    // Calculează timpii rămași pentru fiecare componentă
    uint8_t carsLeftSeconds = 0;
    uint8_t carsRightSeconds = 0;
    uint8_t pedestriansSeconds = 0;
    
    const char* carsLeftState = "";
    const char* carsRightState = "";
    const char* pedestriansState = "";
    
    switch (sequenceState)
    {
        case PedestrianSequenceState::IDLE:
            // Masinile au verde, pietonii au rosu
            carsLeftState = "GREEN";
            carsRightState = "GREEN";
            pedestriansState = "RED";
            carsLeftSeconds = secondsRemaining;
            carsRightSeconds = secondsRemaining;
            pedestriansSeconds = 0; // Nu au verde
            break;
            
        case PedestrianSequenceState::TRANSITION_TO_YELLOW:
            // Masinile au galben, pietonii au rosu
            carsLeftState = "YELLOW";
            carsRightState = "YELLOW";
            pedestriansState = "RED";
            carsLeftSeconds = secondsRemaining;
            carsRightSeconds = secondsRemaining;
            pedestriansSeconds = 0; // Nu au verde
            break;
            
        case PedestrianSequenceState::PEDESTRIAN_CROSSING:
            // Masinile au rosu, pietonii au verde (sau palpitant)
            carsLeftState = "RED";
            carsRightState = "RED";
            
            if (pedState == PedestrianLightState::BLINKING)
            {
                pedestriansState = "GREEN (BLINKING)";
            }
            else
            {
                pedestriansState = "GREEN";
            }
            
            carsLeftSeconds = secondsRemaining;
            carsRightSeconds = secondsRemaining;
            pedestriansSeconds = secondsRemaining;
            break;
            
        default:
            carsLeftState = "UNKNOWN";
            carsRightState = "UNKNOWN";
            pedestriansState = "UNKNOWN";
            break;
    }
    
    snprintf(buffer, sizeof(buffer),
        "[INFO] === Traffic Status ===\r\n"
        "  Cars LEFT:  %s | Time remaining: %u s\r\n"
        "  Cars RIGHT: %s | Time remaining: %u s\r\n"
        "  Pedestrians: %s | Green time remaining: %u s\r\n"
        "[DEBUG] Sequence state: %u | Total button presses: %u\r\n"
        "===========================\r\n",
        carsLeftState, carsLeftSeconds,
        carsRightState, carsRightSeconds,
        pedestriansState, pedestriansSeconds,
        (uint8_t)sequenceState, pressCount);
    UsartDriver::send(buffer);
}

bool PedestrianButtonManager::isButtonPressed() const
{
    return buttonRight->isPressed() || buttonLeft->isPressed();
}

uint8_t PedestrianButtonManager::getSecondsRemaining() const
{
    return secondsRemaining;
}

void PedestrianButtonManager::onStatusUpdate()
{
    // Nu mai folosim timer-ul de status
}

void PedestrianButtonManager::updateSecondsRemaining()
{
    // Nu mai este necesar - secunde rămase sunt actualizate în startSequence()
}

void PedestrianButtonManager::handleGetStatus()
{
    if (g_pedestrianManager)
    {
        g_pedestrianManager->logFullStatus();
    }
    else
    {
        UsartDriver::send("ERR: Pedestrian button manager not initialized.\r\n");
    }
}

void PedestrianButtonManager::handleNight()
{
    if (g_pedestrianManager)
    {
        g_pedestrianManager->nightModeActive = true;
        g_pedestrianManager->nightToggleCounter = 0;
        
        g_pedestrianManager->stopAllTimers();
        
        g_pedestrianManager->pedestrianLights->turnOffAll();
        
        g_pedestrianManager->trafficLights->setState(TrafficLightState::YELLOW);
        
        UsartDriver::send("[NIGHT] Night mode activated: Pedestrian lights OFF, Yellow lights blinking (500ms).\r\n");
    }
    else
    {
        UsartDriver::send("ERR: PedestrianButtonManager not initialized.\r\n");
    }
}

void PedestrianButtonManager::handleDay()
{
    if (g_pedestrianManager)
    {
        g_pedestrianManager->nightModeActive = false;
        g_pedestrianManager->nightToggleCounter = 0;
        
        g_pedestrianManager->stopAllTimers();
        
        g_pedestrianManager->trafficLights->setState(TrafficLightState::GREEN);
        
        g_pedestrianManager->sequenceState = PedestrianSequenceState::IDLE;
        
        g_pedestrianManager->setButtonsEnabled(true);
        
        g_pedestrianManager->buzzers->setState(false);
        
        g_pedestrianManager->pedestrianLights->setState(PedestrianLightState::RED);
        
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