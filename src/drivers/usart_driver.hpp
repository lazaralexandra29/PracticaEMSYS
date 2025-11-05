#ifndef PRACTICAEMSYS_SRC_DRIVERS_USART_DRIVER_HPP
#define PRACTICAEMSYS_SRC_DRIVERS_USART_DRIVER_HPP

#include <stdint.h>

enum class UsartBaudRate : uint32_t 
{
    BR9600 = 9600,
    BR19200 = 19200,
    BR38400 = 38400,
    BR57600 = 57600,
    BR115200 = 115200
};

enum class UsartParity : uint8_t 
{
    NONE = 0,
    EVEN = 1,
    ODD  = 2
};

enum class UsartStopBits : uint8_t 
{
    ONE = 1,
    TWO = 2
};

enum class UsartErrorCode : int8_t 
{
    SUCCESS = 0,
    INVALID_BAUDRATE = -1,
    INVALID_PARITY = -2,
    INVALID_STOP_BITS = -3,
    TRANSMIT_ERROR = -4,
    RECEIVE_ERROR = -5
};

class UsartStatus 
{
   public:
    UsartStatus(UsartErrorCode errorCode = UsartErrorCode::SUCCESS, uint8_t receivedByte = 0)
        : m_errorCode(errorCode), byte_received_(receivedByte) {}

    UsartErrorCode GetErrorCode() const 
    {
        return m_errorCode;
    }

    uint8_t GetReceivedByte() const 
    {
        return byte_received_;
    }

    bool IsSuccess() const 
    {
        return m_errorCode == UsartErrorCode::SUCCESS;
    }

    private:
     const UsartErrorCode m_errorCode;
     const uint8_t byte_received_ = 0;
};

class UsartDriver 
{
    public:
     UsartStatus Init(UsartBaudRate baudRate, UsartParity parity, UsartStopBits stopBits);
     UsartStatus TransmitByte(uint8_t data);
     UsartStatus ReceiveByte();
};

#endif