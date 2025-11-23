#ifndef APP_HARDWARE_UART_INTERFACE_HPP
#define APP_HARDWARE_UART_INTERFACE_HPP

#include "interfaces/iuart_interface.hpp"
#include "drivers/usart_driver.hpp"
#include <stdint.h>

class HardwareUartInterface : public IUartInterface
{
public:
    HardwareUartInterface() = default;

    bool Init(UsartBaudRate baud_rate, UsartParity parity, UsartStopBits stop_bits);

    void Send(const char* message) override;
    bool ReceiveLine(char* buffer, uint8_t length) override;
};

#endif





