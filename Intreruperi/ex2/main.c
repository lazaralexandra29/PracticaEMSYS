#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define BUTTON1_PIN PD2
#define BUTTON2_PIN PD3
#define LED1_PIN PD4
#define LED2_PIN PD5
#define LED3_PIN PD6
#define LED4_PIN PD7

void setPin(volatile uint8_t *port, uint8_t pin){
	*port |=  (1 << pin);
}

void resetPin(volatile uint8_t *port, uint8_t pin){
	*port &=  ~(1 << pin);
}

void init_devices()
{
	//LED-uri ca output
	DDRD = (1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN) | (1 << LED4_PIN);
	
	//Rezistenta de pull-up pentru butoane, LED-uri aprinse
	PORTD = (1 << BUTTON1_PIN) | (1 << BUTTON2_PIN) | (1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN) | (1 << LED4_PIN);
	
	EICRA |= (1 << ISC01) | (1 << ISC11);
	EIMSK |= (1 << INT0) | (1 << INT1);
	
	sei();
}

void OnLEDs() {
	setPin(&PORTD, LED1_PIN);
	setPin(&PORTD, LED2_PIN);
	setPin(&PORTD, LED3_PIN);
	setPin(&PORTD, LED4_PIN);
}

void OffLEDs() {
	resetPin(&PORTD, LED1_PIN);
	resetPin(&PORTD, LED2_PIN);
	resetPin(&PORTD, LED3_PIN);
	resetPin(&PORTD, LED4_PIN);
}

ISR (INT0_vect){
	OffLEDs();
}

ISR (INT1_vect){
	 OnLEDs();
}

int main(void)
{
    init_devices();
    while (1);
}



