#ifdef UNIT_TEST
#include "app/uart_interface.hpp"
#include <stdio.h>
#include <string.h>

class MockUartInterface : public IUartInterface
{
public:
    MockUartInterface()
        : has_pending_command_(false)
    {
        ClearBuffer();
    }

    void LoadIncomingCommand(const char* command)
    {
        if (command == nullptr)
        {
            return;
        }
        size_t length = strlen(command);
        if (length >= sizeof(incoming_command_))
        {
            length = sizeof(incoming_command_) - 1;
        }
        memcpy(incoming_command_, command, length);
        incoming_command_[length] = '\0';
        has_pending_command_ = true;
    }

    const char* GetLastSentMessage() const
    {
        return last_sent_message_;
    }

    void Send(const char* message) override
    {
        if (message == nullptr)
        {
            return;
        }
        size_t length = strlen(message);
        if (length >= sizeof(last_sent_message_))
        {
            length = sizeof(last_sent_message_) - 1;
        }
        memcpy(last_sent_message_, message, length);
        last_sent_message_[length] = '\0';
    }

    bool ReceiveLine(char* buffer, uint8_t length) override
    {
        if (!has_pending_command_ || buffer == nullptr || length == 0)
        {
            return false;
        }

        size_t copy_length = strlen(incoming_command_);
        if (copy_length >= length)
        {
            copy_length = length - 1;
        }

        memcpy(buffer, incoming_command_, copy_length);
        buffer[copy_length] = '\0';
        has_pending_command_ = false;
        return true;
    }

private:
    void ClearBuffer()
    {
        incoming_command_[0] = '\0';
        last_sent_message_[0] = '\0';
    }

    bool has_pending_command_;
    char incoming_command_[64];
    char last_sent_message_[64];
};

class CommandService
{
public:
    explicit CommandService(IUartInterface& uart_interface)
        : uart_interface_(uart_interface)
    {
    }

    void ProcessIncomingCommand()
    {
        char command_buffer[64];
        if (!uart_interface_.ReceiveLine(command_buffer, sizeof(command_buffer)))
        {
            return;
        }

        if (strcmp(command_buffer, "ping") == 0)
        {
            uart_interface_.Send("pong\r\n");
        }
        else if (strcmp(command_buffer, "status") == 0)
        {
            uart_interface_.Send("status:ok\r\n");
        }
        else
        {
            uart_interface_.Send("err:unknown\r\n");
        }
    }

    void SendNotification(const char* notification)
    {
        uart_interface_.Send(notification);
    }

private:
    IUartInterface& uart_interface_;
};

int main()
{
    MockUartInterface mock_uart;
    CommandService command_service(mock_uart);

    mock_uart.LoadIncomingCommand("ping");
    command_service.ProcessIncomingCommand();
    printf("Response to ping: %s", mock_uart.GetLastSentMessage());

    mock_uart.LoadIncomingCommand("status");
    command_service.ProcessIncomingCommand();
    printf("Response to status: %s", mock_uart.GetLastSentMessage());

    mock_uart.LoadIncomingCommand("night");
    command_service.ProcessIncomingCommand();
    printf("Response to night: %s", mock_uart.GetLastSentMessage());

    command_service.SendNotification("system:ready\r\n");
    printf("Notification: %s", mock_uart.GetLastSentMessage());

    return 0;
}
#endif  // UNIT_TEST
