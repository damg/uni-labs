/*
 * radarfalle.c
 *
 * Created: 28.10.2011 12:47:12
 *  Author: Thor
 */ 
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define RES_ 277.7778

void init_rs232(unsigned int ubrr);
int uart_getchar(FILE *stream);
int uart_putchar( char c, FILE *stream );
void start_measuring(void);
void stop_measuring(void);


#define FOSC 3686400UL
#define BAUD 9600UL
#define MYUBRR (FOSC/16/BAUD-1)

FILE rs232f = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void)
{
	PORTB = 0xFF;
	PORTD = 0xFF;
	
	DDRB = 0x00;
	DDRB = 0x00;
	
	cli();
	
	init_rs232(MYUBRR);
	
	TCCR1A=0x00;
	
	TCCR1B=0x80;// config : capture on falling edge
	TCNT1=0x00;
	TIMSK|=0x20;
	
	GICR |= (1<< INT0);
	MCUCR |= (1 << ISC01);
		
	sei();
	
	fprintf(&rs232f, "run\n");
	
    while(1)
    {
    }
	
	return 0;;
}

ISR (INT0_vect)
{
	if ( TCNT1 == 0x00 )	// Timer (einmal) starten
	{
		fprintf(&rs232f, "start\n");

		start_measuring();
	}	
}



ISR (TIMER1_CAPT_vect)
{
	uint16_t icr = ICR1;
	double msec = ( RES_ * icr ) / 1000;
//	fprintf(&rs232f, "elapsed_time: %8.2f\n", time_);

	double v = (46.0 * 10.0) / msec;
	double kmh = v * 3.6;
	
//	int msi = (int)msec;
//	int vi = (int)v;
//	int kmhi = (int)kmh;

	if ( TCNT1 != 0x00 )
	{
		fprintf(&rs232f, "elapsed_time: %.2f ms, speed = %.2f m/s bzw %.2f km/h\n"
								, msec, v, kmh );
		stop_measuring();
	}
}

void start_measuring(void)
{
	//TCCR1B &=~(0x05);// timer off
	TCNT1=0x00; // timer reset
	TCCR1B |= 0x05;	// timer om with prescaler 1024
}

void stop_measuring(void)
{
	TCCR1B &=~(0x05);
	TCNT1 = 0x00;
}

void init_rs232(unsigned int ubrr)
{
	UBRRH = (unsigned char) (ubrr>>8);
	UBRRL = (unsigned char) ubrr;
	
	UCSRB = (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << URSEL) | (1<<UCSZ1) | (1 << UCSZ0); // 8N1
}

int uart_putchar( char c, FILE *stream )
{
	if( c == '\n' )
	uart_putchar( '\r', stream );
	loop_until_bit_is_set( UCSRA, UDRE );
	UDR = c;
	return 0;
}

int uart_getchar(FILE *stream)
{
	unsigned char ch;
	while (!(UCSRA & (1<<RXC)));
	ch=UDR;
	/* Echo the output back to the terminal */
//	uart_putchar(ch,stream);
	return ch;
}

