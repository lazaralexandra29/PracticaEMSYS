#ifndef PRACTICAEMSYS_SRC_DRIVERS_TIMER_DRIVER_HPP
#define PRACTICAEMSYS_SRC_DRIVERS_TIMER_DRIVER_HPP

#include <stdint.h>

enum class TimerMode : uint8_t 
{
    NORMAL = 0,
    CTC = 1
};

enum class Prescaler : uint8_t 
{
    DIV_1 = 0,
    DIV_8 = 1,
    DIV_64 = 2,
    DIV_256 = 3,
    DIV_1024 = 4
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
     TimerConfiguration(TimerMode mode, Prescaler prescaler)
        : m_mode(mode), m_prescaler(prescaler)
    {}

    TimerMode GetMode() const 
    {
        return m_mode;
    }

    Prescaler GetPrescaler() const 
    {
        return m_prescaler;
    }

    private:
     const TimerMode m_mode;
     const Prescaler m_prescaler;
};

class TimerDriver
{
    public:
        TimerDriver() : base_timer_counter_value_(0) {};
        
        //TimerStatus Init(const TimerConfiguration& timer_config);

        TimerStatus InitTimer1(const TimerConfiguration& config);

        uint8_t CreateTimerSoftware(); //poate sa primeasca cv
        TimerStatus RegisterPeriodicCallback(uint8_t timer_id, void (*callback)(), uint32_t period_ms);
        TimerStatus UnregisterPeriodicCallback(uint8_t timer_id);
        TimerStatus Stop(uint8_t timer_id);   

    protected:
     uint16_t base_timer_counter_value_;

};

#endif