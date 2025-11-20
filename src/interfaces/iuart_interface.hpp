#ifndef INTERFACES_IUART_INTERFACE_HPP
#define INTERFACES_IUART_INTERFACE_HPP

#include <stdint.h>

class IUartInterface
{
public:
    virtual ~IUartInterface() = default;

    virtual void Send(const char* message) = 0;
    virtual bool ReceiveLine(char* buffer, uint8_t length) = 0;
};

#endif


