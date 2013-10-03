/*
 *                 ATmega128 Real-Time Clock example
 *
 * Description : Every 1 second an interrupt is triggered and real
 *               time clock incremented. Timer 1 is used in compare
 *               match mode to trigger interrupt.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: rtc.c,v 1.2 2006/12/09 11:40:56 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

struct time
{
	uint8_t tm_hour;	/* hours [0-23] */
	uint8_t tm_min;		/* minutes [0-59] */
	uint8_t tm_sec;		/* seconds [0-59] */
};

/* global variables */
volatile struct time RTC={0,0,0};

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);

int main(void)
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, NULL, _FDEV_SETUP_RW);
	uint8_t sec=99;		/* second flag */

	stdout = stdin = stderr =  &mystdout;

	DDRD = 0xFF;		/* set port D as output */
	PORTD = 0xFF;		/* turn off all port D LEDs */

	/* set timer 1 control register B (set clock source)
	 * set prescaler 1024 and clear timer on compare match
         */
	TCCR1B = _BV(CS12)|_BV(CS10)|_BV(WGM12);

	/* set output compare register 1 A - OCR1A = OCR1AH + OCR1AL
	 * set F_CPU/1024 = 14745600/1024 = 14400
	 */
	OCR1A = ((F_CPU/1024) - 1 );

	TIMSK  = _BV(OCIE1A);	/* enable timer 1 output compare A match interrupt */

	sei();			/* global interrupt enable */

				/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	printf("\n--== Real-Time Clock  ==--\n\n");

	while(1)
	{
		if( RTC.tm_sec != sec )
		{
			sec = RTC.tm_sec;            /* display current time */
			printf("\r       %02u:%02u:%02u", RTC.tm_hour, RTC.tm_min, RTC.tm_sec );
		}
	};		/* loop for ever */
}

/*!
 * \brief Output compare A match interrupt service routine.
 *
 */
ISR(TIMER1_COMPA_vect)
{
	PORTD ^= 1;		/* toggle LED */

	if (++RTC.tm_sec > 59)
	{ RTC.tm_sec = 0;
		if (++RTC.tm_min > 59)
		{ RTC.tm_min = 0;
			if (++RTC.tm_hour > 23)
				RTC.tm_hour = 0;
		}
	}
}

/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baud rate generator value.
 */
void ser_init(uint16_t baud)
{
	/* set baud rate */
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t) baud;

	/* enable receiver and transmitter */
	UCSR0B = (1<<RXEN)|(1<<TXEN);

	/* set frame format: 8 data, 2 stop bits */
	UCSR0C = (1<<USBS)|(3<<UCSZ0);
}

/*!
 * \brief Writes the character c to serial port (UART0).
 *
 * \param c  character to write.
 */
int ser_putch(char c, FILE *stream)
{
	if (c == '\n')
		ser_putch('\r', stream);

	/* wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) );

	/* put data into buffer and sends the data */
	UDR0 = c;
	return 0;
}

