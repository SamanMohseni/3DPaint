#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#define BAUD 19200
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define true 1
#define false 0

#define SW_PORT PORTD
#define SW_PIN PIND
volatile uint8_t sw = PD2;
volatile uint8_t photocell_number, photocell[3] = {PC0, PC1, PC2}, extra_bits, next_byte, shift[3] = {0, 2, 4}, first_time;


static inline void initUSART(void) {
	UBRR0H = UBRRH_VALUE;                        /* defined in setbaud.h */
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif
	/* Enable USART transmitter/receiver */
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << TXCIE0); /* Enable TX Complete interrupt */
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit */
}

uint8_t receiveByte(void) {
	loop_until_bit_is_set(UCSR0A, RXC0);       /* Wait for incoming data */
	return UDR0;                                /* return register value */
}

static inline void initADC(void){
	// Set up ADC
	ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC */
	ADCSRA |= (1 << ADPS2) | (1 << ADPS0);   /* ADC clock prescaler  /32 */
	ADCSRA |= (1 << ADEN) | (1 << ADIE); /* enable ADC and ADC interrupt */
}

static inline void initTimer1(void) {
	TCCR1B |= (1 << WGM12);                                  /* CTC mode */
	TCCR1B |= (1 << CS11) | (1 << CS10);            /* Clock at CPU/64 frequency */
	TIMSK1 |= (1 << OCIE1A);          /* enable output compare interrupt */
}

ISR(USART_TX_vect){
	if(photocell_number < 4){
		if(photocell_number == 3){
			UDR0 = extra_bits & (((SW_PIN >> sw) & 1) << 7);   /* send extra_bits with switch data included */
			photocell_number++;
		}
		else{
			UDR0 = next_byte;                                      /* send data */
			if(photocell_number != 2){
				photocell_number++;
				ADMUX = (0xf0 & ADMUX) | photocell[photocell_number];
				ADCSRA |= (1 << ADSC);                      /* start conversion */
			}
			else{
				photocell_number++;
			}
		}
	}
}

ISR(ADC_vect){
	next_byte = ADCL;
	extra_bits |= (ADCH % 4) << shift[photocell_number];
	if(first_time){
		UDR0 = next_byte;                                     /* send data */
		photocell_number++;
		ADMUX = (0xf0 & ADMUX) | photocell[photocell_number];
		ADCSRA |= (1 << ADSC);                          /* start conversion */
		first_time = false;
	}
}

ISR(TIMER1_COMPA_vect) {
	photocell_number = 0;
	extra_bits = 0;
	first_time = true;
	ADMUX = (0xf0 & ADMUX) | photocell[photocell_number];
	ADCSRA |= (1 << ADSC);                              /* start conversion */
}

#define send_delay_cycle 1250 /* 100 data per second */
#define start_command 's'
int main(void) {
	clock_prescale_set(clock_div_1);
	SW_PORT |= (1 << sw); /* set pullup resistor on our switch */
	initUSART();
	initADC();
	initTimer1();
	OCR1A = send_delay_cycle;
	while(receiveByte() != start_command);
	sei();
	while(1);
	return 0;
}
