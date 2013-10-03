/*
 *           ATmega128 USART - Data Register Empty Interrupt example
 *
 * Description : The USART0 data register empty (UDRE0) is enabled and
 *               interrupt will be executed when the transmit buffer is
 *               ready to receive new data. PD0 output is turned on every
 *               time an UDRE0 interrupt is enabled.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: uart.c,v 1.1 2006/12/09 11:38:59 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* global variables */
volatile char *SER_TXD_BUF;	/* serial transmit buffer (pointer) */

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
void ser_puts(char *s);

int main(void)
{
	uint8_t i;

	DDRD = 0xFF;		/* set port D as output */
	PORTD = 0xFF;		/* turn off all port D LEDs */

				/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	sei();			/* global interrupt enable */

	ser_puts("\r\n---== ATmega128 Data Register Empty Interrupt Example ==---\r\n\n");

	while(1)
	{
		for(i=0;i<200;i++)	/* wait 2 seconds */
			_delay_ms(10);
					/* send string via interrupt */
		ser_puts("-> This is an interrupt transmitted string.\r\n");
	};
}

/*!
 * \brief Data register empty interrupt service routine.
 */
ISR(USART0_UDRE_vect)
{
	if(*SER_TXD_BUF)
	{
		PORTD = 0xFE;		/* turn on LED */
		UDR0 = *SER_TXD_BUF++;	/* send character */
	}
	else
	{
				/* disable UDRE interrupt */
		UCSR0B &= ~(_BV(UDRE0));
		PORTD = 0xFF;		/* turn off LED */
	}
}

/*!
 * \brief Writes the string to serial port (USART0).
 *
 * \param s  string to write.
 */
void ser_puts(char *s)
{
	SER_TXD_BUF = s;	/* set data to send */
	UCSR0B |= _BV(UDRE0);	/* enable UDRE interrupt */
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
	UCSR0B = _BV(RXEN0)|_BV(TXEN0);

	/* set frame format: 8 data, 2 stop bits */
	UCSR0C = _BV(USBS0)|_BV(UCSZ01)|_BV(UCSZ00);
}

