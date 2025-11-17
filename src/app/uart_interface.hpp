#ifndef APP_UART_INTERFACE_HPP
#define APP_UART_INTERFACE_HPP

#include "drivers/usart_driver.hpp"
#include <stdint.h>

class IUartInterface
{
public:
    virtual ~IUartInterface() = default;

    virtual void Send(const char* message) = 0;
    virtual bool ReceiveLine(char* buffer, uint8_t length) = 0;
};

class HardwareUartInterface : public IUartInterface
{
public:
    HardwareUartInterface() = default;

    bool Init(UsartBaudRate baud_rate, UsartParity parity, UsartStopBits stop_bits);

    void Send(const char* message) override;
    bool ReceiveLine(char* buffer, uint8_t length) override;
};

#endif


