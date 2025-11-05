#include "drivers/usart_driver.hpp"
#include <avr/io.h>

UsartStatus UsartDriver::Init(UsartBaudRate baudRate, UsartParity parity, UsartStopBits stopBits)
{
    uint16_t ubrr_value = (F_CPU / (16UL * static_cast<uint32_t>(baudRate))) - 1;

    if (ubrr_value > 4095)
        return UsartStatus(UsartErrorCode::INVALID_BAUDRATE);

    UBRR0H = static_cast<uint8_t>(ubrr_value >> 8);
    UBRR0L = static_cast<uint8_t>(ubrr_value & 0xFF);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0); 

    uint8_t config = 0;

    switch (parity)
    {
        case UsartParity::NONE: config &= ~((1 << UPM01) | (1 << UPM00)); break;
        case UsartParity::EVEN: config |= (1 << UPM01); break;
        case UsartParity::ODD:  config |= (1 << UPM01) | (1 << UPM00); break;
        default: return UsartStatus(UsartErrorCode::INVALID_PARITY);
    }

    switch (stopBits)
    {
        case UsartStopBits::ONE: config &= ~(1 << USBS0); break;
        case UsartStopBits::TWO: config |= (1 << USBS0); break;
        default: return UsartStatus(UsartErrorCode::INVALID_STOP_BITS);
    }

    config |= (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0C = config;

    return UsartStatus(UsartErrorCode::SUCCESS);
}

UsartStatus UsartDriver::TransmitByte(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0))) {} 

    UDR0 = data;

    while (!(UCSR0A & (1 << TXC0))) {}

    return UsartStatus(UsartErrorCode::SUCCESS);
}

UsartStatus UsartDriver::ReceiveByte()
{
    while (!(UCSR0A & (1 << RXC0))) {} 

    uint8_t received = UDR0;
    return UsartStatus(UsartErrorCode::SUCCESS, received);
}