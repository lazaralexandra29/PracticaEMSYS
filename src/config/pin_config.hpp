#ifndef PIN_CONFIG_HPP
#define PIN_CONFIG_HPP

#include <avr/io.h> 

// Butoane pentru pietoni
#define BUTTON_RIGHT_PORT   &PORTD  
#define BUTTON_RIGHT_PIN    2

#define BUTTON_LEFT_PORT    &PORTD 
#define BUTTON_LEFT_PIN     3

// Semafoare pentru mașini (stânga)
#define TRAFFIC_LIGHT_LEFT_RED_PORT    &PORTB
#define TRAFFIC_LIGHT_LEFT_RED_PIN     0

#define TRAFFIC_LIGHT_LEFT_YELLOW_PORT &PORTB
#define TRAFFIC_LIGHT_LEFT_YELLOW_PIN  1

#define TRAFFIC_LIGHT_LEFT_GREEN_PORT  &PORTB
#define TRAFFIC_LIGHT_LEFT_GREEN_PIN   2

// Semafoare pentru mașini (dreapta)
#define TRAFFIC_LIGHT_RIGHT_RED_PORT    &PORTB
#define TRAFFIC_LIGHT_RIGHT_RED_PIN     3

#define TRAFFIC_LIGHT_RIGHT_YELLOW_PORT &PORTB
#define TRAFFIC_LIGHT_RIGHT_YELLOW_PIN  4

#define TRAFFIC_LIGHT_RIGHT_GREEN_PORT  &PORTB
#define TRAFFIC_LIGHT_RIGHT_GREEN_PIN   5

// Semafoare pentru pietoni (stanga)
#define PEDESTRIAN_LEFT_RED_PORT   &PORTD
#define PEDESTRIAN_LEFT_RED_PIN     4

#define PEDESTRIAN_LEFT_GREEN_PORT &PORTD
#define PEDESTRIAN_LEFT_GREEN_PIN  5

// Semafoare pentru pietoni (dreapta)
#define PEDESTRIAN_RIGHT_RED_PORT   &PORTD
#define PEDESTRIAN_RIGHT_RED_PIN    6

#define PEDESTRIAN_RIGHT_GREEN_PORT &PORTD
#define PEDESTRIAN_RIGHT_GREEN_PIN  7

// Buzzer pentru pietoni
#define BUZZER_PORT   &PORTC
#define BUZZER_PIN    1

// Senzor de lumina
#define LIGHT_SENSOR_ADC_CHANNEL  0  

#endif
