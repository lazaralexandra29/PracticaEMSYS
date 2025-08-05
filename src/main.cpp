#define F_CPU16000000UL

#include <avr/io.h>
#include <avr/delay.h>

int main(void)
{
	DDRB=0xFF;
	PORTB=0x00;
	
	while(1)
	{
		PORTB ^=(1<<PORTB5);
		_delay_ms(500);
	}
}