#ifndef PRACTICAEMSYS_SRC_DRIVERS_TIMER_DRIVER_HPP
#define PRACTICAEMSYS_SRC_DRIVERS_TIMER_DRIVER_HPP

#include <stdint.h>

enum class TimerMode : uint8_t {
    NORMAL = 0,
    CTC = 1
};

enum class Prescaler : uint8_t {
    DIV_0 = 0, 
    DIV_1 = 1,
    DIV_8 = 2,
    DIV_32 = 3,
    DIV_64 = 4,
    DIV_128 = 5,
    DIV_256 = 6,
    DIV_1024 = 7
};

enum class TimerErrorCode : int8_t
{
    SUCCESS = 0, 
    INVALID_TIMER_ID = -1,
    INVALID_PRESCALER = -2,
    TIMER_NOT_INITIALIZED = -3,
    INVALID_COMPARE_VALUE = -4,
    INVALID_TIMER_MODE = -5
};

class TimerStatus
{
    public:
      TimerStatus(TimerErrorCode errorCode = TimerErrorCode::SUCCESS) 
        : m_errorCode(errorCode) {}
    
        TimerErrorCode GetErrorCode() const 
        { 
            return m_errorCode; 
        }

        bool IsSuccess() const 
        { 
            return m_errorCode == TimerErrorCode::SUCCESS; 
        }

    private:
      const TimerErrorCode m_errorCode;
};

class TimerConfiguration
{
    public:
      TimerConfiguration(uint8_t id, TimerMode mode, Prescaler prescaler, uint16_t compareValue = 0) 
        : m_timerId(id), m_mode(mode), m_prescaler(prescaler), m_compareValue(compareValue)
        {}
        
        uint8_t GetTimerId() const 
        { 
            return m_timerId; 
        }

        TimerMode GetMode() const 
        { 
            return m_mode; 
        }
        
        Prescaler GetPrescaler() const 
        { 
            return m_prescaler; 
        }

    private:
        const uint8_t m_timerId;
        const TimerMode m_mode;
        const Prescaler m_prescaler;
        const uint16_t m_compareValue;
};

class TimerDriver{
    public:
        TimerStatus Init(const TimerConfiguration& desc);
        
        TimerStatus SetCompareValue(uint8_t timer_id, uint16_t value);
        
        TimerStatus CheckElapsed(uint8_t timer_id, bool& elapsed);

        TimerStatus Stop(uint8_t timer_id);   

        TimerStatus Reset(uint8_t timer_id);   
};

#endif