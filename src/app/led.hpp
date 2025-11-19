#ifndef LED_HPP
#define LED_HPP

#include "drivers/gpio_driver.hpp"
#include "app/logger.hpp"
#include <stdint.h>
#include <stdbool.h>

class Led
{
public:
    Led(volatile uint8_t* port, uint8_t pin, ILogger* logger = nullptr);

    void Init();
    void SetState(bool on);
    bool GetState() const;
    void Toggle();

    static void SetLogger(ILogger* logger);

private:
    GpioDriver gpio_;
    PinDescription led_pin_;
    ILogger* logger_;

    static ILogger* static_logger_;
    ILogger* GetLogger() const;
    static ILogger* GetStaticLogger();
};

#endif
