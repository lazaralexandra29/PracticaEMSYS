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
      TimerConfiguration(m_modeode mode, Prescaler prescaler)
        : m_mode(mode), m_prescaler(prescaler)
      {

      }

    private:
        const TimerMode m_mode;
        const Prescaler m_prescaler;
};

class TimerDriver{
    public:
    
        TimerDriver() : base_timer_counter_value_(0)
        {

        }

        // Metoda de initializare a unui timer hardware;
        // TIMERA, cel pe 16 bit;
        // Baza de timp e constanta, la 1 ms;
        TimerStatus Init(const TimerConfiguration& timer_config);

        uint8_t CreateTimerSoftware();
        TimerStatus RegisterPeriodicCallbakc(uint8_t timer_id, void (*callback)(), uint32_t period_ms);
        TimerStatus UnregisterPeriodicCallback(uint8_t timer_id);
        TimerStatus Stop(uint8_t timer_id);   

        protected:
            uint16_t base_timer_counter_value_;



        
};

ISR(TIMER1_COMPA_vect)
{
    if( MAX(UINT16_MAX == base_timer_counter_value_))
    {
        base_timer_counter_value_ = 0;
    }
    else
    {
        base_timer_counter_value_++;
    }
}

#endif