#ifndef PIN_CONFIG_HPP
#define PIN_CONFIG_HPP

#include <avr/io.h> 

// Legacy LEDs (compatibilitate)
#define LED_GREEN_PORT &PORTC
#define LED_GREEN_PIN  3  

#define LED_RED_PORT   &PORTC
#define LED_RED_PIN    4  

#define LED_YELLOW_PORT &PORTC
#define LED_YELLOW_PIN  5  

// Butoane pentru pietoni
#define BUTTON_RIGHT_PORT   &PORTD  
#define BUTTON_RIGHT_PIN    2

#define BUTTON_LEFT_PORT    &PORTD 
#define BUTTON_LEFT_PIN     3

// Legacy definition (compatibilitate)
#define BUTTON_PORT   BUTTON_RIGHT_PORT
#define BUTTON_PIN    BUTTON_RIGHT_PIN

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

// Legacy definitions (compatibilitate cu BuzzerManager)
#define BUZZER_LEFT_PORT   BUZZER_PORT
#define BUZZER_LEFT_PIN    BUZZER_PIN
#define BUZZER_RIGHT_PORT  BUZZER_PORT
#define BUZZER_RIGHT_PIN   BUZZER_PIN

// Senzor de lumina
#define LIGHT_SENSOR_ADC_CHANNEL  0  

#endif
