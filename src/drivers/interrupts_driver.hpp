#ifndef INTERRUPTS_DRIVER_HPP
#define INTERRUPTS_DRIVER_HPP

#include <stdint.h>

typedef void (*InterruptCallback_t)(void);

enum class InterruptSource : uint8_t {
    EXT_INT0 = 0,
    EXT_INT1 = 1,
    PCINT0 = 2, // Port B
    PCINT1 = 3, // Port C
    PCINT2 = 4  // Port D
};

enum class InterruptTrigger : uint8_t {
    LOW_LEVEL = 0,
    ANY_CHANGE = 1,
    FALLING_EDGE = 2,
    RISING_EDGE = 3
};

enum class InterruptErrorCode : int8_t {
    SUCCESS = 0,
    NULL_CALLBACK = -1,
    INVALID_SOURCE = -2,
    UNSUPPORTED_TRIGGER = -3
};

class InterruptStatus {
public:
    InterruptStatus(InterruptErrorCode code = InterruptErrorCode::SUCCESS)
        : m_code(code) {}

    InterruptErrorCode GetErrorCode() const { return m_code; }
    bool IsSuccess() const { return m_code == InterruptErrorCode::SUCCESS; }

private:
    const InterruptErrorCode m_code;
};

class InterruptsDriver {
public:
    InterruptsDriver();

    InterruptStatus Attach(InterruptSource source, InterruptCallback_t callback, InterruptTrigger trigger = InterruptTrigger::FALLING_EDGE);
    void EnableGlobalInterrupts();

private:
    static InterruptCallback_t ext_int0_callback;
    static InterruptCallback_t ext_int1_callback;
    static InterruptCallback_t pcint0_callback;
    static InterruptCallback_t pcint1_callback;
    static InterruptCallback_t pcint2_callback;
};

#endif