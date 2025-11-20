#include "app/hardware_uart_interface.hpp"

bool HardwareUartInterface::Init(UsartBaudRate baud_rate, UsartParity parity, UsartStopBits stop_bits)
{
    return UsartDriver::Init(baud_rate, parity, stop_bits).IsSuccess();
}

void HardwareUartInterface::Send(const char* message)
{
    if (message == nullptr)
    {
        return;
    }
    UsartDriver::send(message);
}

bool HardwareUartInterface::ReceiveLine(char* buffer, uint8_t length)
{
    if (buffer == nullptr || length == 0)
    {
        return false;
    }

    return UsartDriver::receiveLineNonBlocking(buffer, length);
}

