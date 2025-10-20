#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>

enum class TimerMode : uint8_t {
    NORMAL = 0,
    CTC = 1
};

enum class Prescaler : uint8_t {
    DIV_0 = 0,
    DIV_1 = 1,
    DIV_8 = 2,
    DIV_64 = 3,
    DIV_256 = 4,
    DIV_1024 = 5
};

class TimerDriver{
    public:
    int8_t Init(uint8_t timer_id, TimerMode mode, Prescaler prescaler);
    int8_t SetCompareValue(uint8_t timer_id, uint16_t value);
    bool HasElapsed(uint8_t timer_id);
};

#endif