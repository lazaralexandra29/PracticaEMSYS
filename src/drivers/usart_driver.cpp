#include "usart_driver.hpp"
#include <avr/io.h>

UsartStatus UsartDriver::Init(const UsartDescription& usartDesc, UsartBaudRate baudRate, UsartParity parity, UsartStopBits stopBits)
{
    uint16_t ubrrValue = static_cast<uint32_t>(F_CPU) / (16UL * static_cast<uint32_t>(baudRate)) - 1;
    
    *(usartDesc.GetUBRR()) = static_cast<uint8_t>(ubrrValue);

    // Enable transmitter and receiver
    *(usartDesc.GetUCSRB()) |= (1 << RXEN0) | (1 << TXEN0);

    // Configure frame format: 8 data bits
    *(usartDesc.GetUCSRC()) |= (1 << UCSZ01) | (1 << UCSZ00);

    // Configure parity
    *(usartDesc.GetUCSRC()) &= ~(1 << UPM01 | 1 << UPM00);
    switch (parity)
    {
        case UsartParity::NONE: break;
        case UsartParity::EVEN: *(usartDesc.GetUCSRC()) |= (1 << UPM01); break;
        case UsartParity::ODD:  *(usartDesc.GetUCSRC()) |= (1 << UPM01) | (1 << UPM00); break;
        default: return UsartStatus(UsartErrorCode::INVALID_PARITY);
    }

    // Configure stop bits
    *(usartDesc.GetUCSRC()) &= ~(1 << USBS0);
    switch (stopBits)
    {
        case UsartStopBits::ONE: break;
        case UsartStopBits::TWO: *(usartDesc.GetUCSRC()) |= (1 << USBS0); break;
        default: return UsartStatus(UsartErrorCode::INVALID_STOP_BITS);
    }

    return UsartStatus(UsartErrorCode::SUCCESS);
}

UsartStatus UsartDriver::TransmitByte(const UsartDescription& usartDesc, uint8_t data)
{
    // Wait until transmit buffer is empty
    while (!(*(usartDesc.GetUCSRA()) & (1 << UDRE0)));

    *(usartDesc.GetUDR()) = data;

    // Wait until transmission is complete
    while (!(*(usartDesc.GetUCSRA()) & (1 << TXC0)));

    return UsartStatus(UsartErrorCode::SUCCESS);
}

UsartStatus UsartDriver::ReceiveByte(const UsartDescription& usartDesc, uint8_t& outData)
{
    // Wait until data is received
    while (!(*(usartDesc.GetUCSRA()) & (1 << RXC0)));

    outData = *(usartDesc.GetUDR());

    return UsartStatus(UsartErrorCode::SUCCESS);
}
