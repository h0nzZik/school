/*
 *                 ATmega128 USART - Receive Complete Interrupt example
 *
 * Description : The USART0 receive complete interrupt (RXCIE0) is enabled and
 *               interrupt will be executed when unread data are available.
 *               PD0 output is toggled every time an interrupt (RXCIEO)
 *               executes.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: uart.c,v 1.2 2006/12/09 11:40:01 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);

int main(void)
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, NULL, _FDEV_SETUP_RW);

	stdout = stdin = stderr =  &mystdout;

	DDRD = 0xFF;		/* set port D as output */
	PORTD = 0xFF;		/* turn off all port D LEDs */

				/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	sei();			/* global interrupt enable */

	printf("\n--== Press any key to see ATmega128 response  ==--\n\n");

	while(1){};		/* loop for ever */
}

/*!
 * \brief Receive complete interrupt service routine.
 */
ISR(USART0_RX_vect)
{
	PORTD ^= 1;		/* toggle LED */

	/*
	 * The receive complete routine must read the received data from
	 * UDR in order to clear the RXC flag, otherwise a new interrupt
	 * will occur once the interrupt routine terminates.
	 */

				/* wait for empty transmit buffer */
	while ( !( UCSR0A & _BV(UDRE0)) );

	UDR0 = UDR0;		/* read and send back the data */
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
	UCSR0B = _BV(RXCIE0)|_BV(RXEN0)|_BV(TXEN0);

	/* set frame format: 8 data, 2 stop bits */
	UCSR0C = _BV(USBS0)|_BV(UCSZ01)|_BV(UCSZ00);
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
	while ( !( UCSR0A & _BV(UDRE0)) );

	/* put data into buffer and sends the data */
	UDR0 = c;
	return 0;
}

