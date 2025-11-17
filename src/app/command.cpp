#include "app/command.hpp"
#include "app/pedestrian_button.hpp"
#include "drivers/usart_driver.hpp"
#include <string.h>
#include <stdlib.h>

Command::Command(PedestrianButton* pedestrian_button)
    : pedestrian_button_(pedestrian_button)
{
}

void Command::ExecuteCommand(const char* command)
{
    if (pedestrian_button_ == nullptr)
    {
        UsartDriver::send("ERR: PedestrianButton dependency missing.\r\n");
        return;
    }

    char buffer[64];
    strncpy(buffer, command, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len-1] == ' ' || buffer[len-1] == '\t' || buffer[len-1] == '\r' || buffer[len-1] == '\n'))
    {
        buffer[len-1] = '\0';
        len--;
    }

    char* token = strtok(buffer, " ");
    if (token == NULL) 
    {
        UsartDriver::send("ERR: No command received.\r\n");
        return;
    }

    if (strcmp(token, "night") == 0 || strcmp(token, "Night") == 0 || strcmp(token, "NIGHT") == 0)
    {
        pedestrian_button_->HandleNightMode();
    }
    else if (strcmp(token, "day") == 0 || strcmp(token, "Day") == 0 || strcmp(token, "DAY") == 0)
    {
        pedestrian_button_->HandleDayMode();
    }
    else if (strcmp(token, "Help") == 0 || strcmp(token, "help") == 0 || strcmp(token, "HELP") == 0)
    {
        UsartDriver::send("\r\n--- Available Commands ---\r\n");
        UsartDriver::send("night - Activate night mode\r\n");
        UsartDriver::send("day - Deactivate night mode\r\n");
        UsartDriver::send("help - Show this help\r\n");
        UsartDriver::send("exit - Exit program\r\n");
        UsartDriver::send("--------------------------\r\n");
    }
    else
    {
        UsartDriver::send("ERR: Unknown command. Type 'help'.\r\n");
    }
}


