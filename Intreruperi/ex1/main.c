#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define BUTTON1_PIN PD2
#define LED0_PIN PD4
#define LED1_PIN PD5

#define LED0_PORT PORTD
#define LED1_PORT PORTD

void pinToggle(volatile uint8_t *port, uint8_t pin){
	*port ^=  1 << pin;
}

void setPin(volatile uint8_t *port, uint8_t pin){
	*port |=  (1 << pin);
}

void resetPin(volatile uint8_t *port, uint8_t pin){
	*port &=  ~(1 << pin);
}

void init_devices()
{
	//LED-uri ca output
	DDRD = (1 << LED0_PIN) | (1 << LED1_PIN);
	
	//Rezistenta de pull-up pentru buton
	PORTD = (1 << BUTTON1_PIN);
	
	EICRA |= (1 << ISC00);
	EIMSK |= (1 << INT0);
	
	sei();
}

ISR (INT0_vect){
	if ( (PIND & (1 << BUTTON1_PIN)) == 0){
		setPin (&LED1_PORT, LED1_PIN);
	}
	else
	{
		resetPin(&LED1_PORT, LED1_PIN);
	}
}


int main(void)
{
    init_devices();
	
	do{
		// toggle pin
		pinToggle(&LED0_PORT, LED0_PIN);
		
		// long time delay - 3 seconds 
		_delay_ms(3000);
	}
	
    while (1);
}

