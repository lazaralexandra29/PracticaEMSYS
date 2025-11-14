#ifndef PIN_CONFIG_HPP
#define PIN_CONFIG_HPP

#include <avr/io.h> 

#define LED_GREEN_PORT &PORTD
#define LED_GREEN_PIN  3

#define LED_RED_PORT   &PORTD
#define LED_RED_PIN    4

#define LED_YELLOW_PORT &PORTD
#define LED_YELLOW_PIN  5

#define BUTTON_PORT   &PORTD
#define BUTTON_PIN    2

#endif