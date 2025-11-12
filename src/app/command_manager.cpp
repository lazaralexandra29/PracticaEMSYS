#include "app/command_manager.hpp"
#include "app/led_manager.hpp"
#include "app/button_manager.hpp" 
#include "drivers/usart_driver.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void CommandManager::executeCommand(const char* command)
{
    char buffer[64];
    strncpy(buffer, command, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char* token = strtok(buffer, " ");
    if (token == NULL) 
    {
        UsartDriver::send("ERR: No command received.\r\n");
        return;
    }

    if (strcmp(token, "SetLED") == 0)
    {
        char* indexStr = strtok(NULL, " "); 
        char* stateStr = strtok(NULL, " ");
        
        if (indexStr && stateStr)
        {
            uint8_t index = atoi(indexStr);
            LedManager::handle(index, stateStr);
        }
        else
        {
            UsartDriver::send("ERR: Usage -> SetLED <index> <ON|OFF|TOGGLE>\r\n");
        }
    }

    else if (strcmp(token, "GetLEDState") == 0)
    {
        char* indexStr = strtok(NULL, " ");
        
        if (indexStr)
        {
            uint8_t index = atoi(indexStr);
            LedManager::reportLedState(index); 
        }
        else
        {
            UsartDriver::send("ERR: Usage -> GetLEDState <index>\r\n");
        }
    }

    else if (strcmp(token, "ButtonInfo") == 0)
    {
        ButtonManager::reportState(); 
    }

    else if (strcmp(token, "Help") == 0)
    {
        UsartDriver::send("\r\n--- Available Commands ---\r\n");
        UsartDriver::send("SetLED <idx> <ON|OFF|TOGGLE>\r\n");
        UsartDriver::send("GetLEDState <idx>\r\n");
        UsartDriver::send("ButtonInfo\r\n");
        UsartDriver::send("--------------------------\r\n");
    }
    
    else
    {
        UsartDriver::send("ERR: Unknown command. Type 'Help'.\r\n");
    }
}