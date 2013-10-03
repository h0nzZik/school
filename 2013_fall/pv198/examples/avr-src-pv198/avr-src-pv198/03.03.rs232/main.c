/*
 *                  R S 2 3 2 - s t d i o - e x a m p l e
 *
 * Description : Simple demo how to use stdio functions for reading and
 *               writing data from/to ATmega128's serial port (UART0).
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.2 2008/11/07 08:32:14 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);

int main( void )
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stdin = stderr =  &mystdout;

					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	while(1)
	{
		printf("Hello world - RAM !\n");
		getchar();

		fprintf(stdout, "Hello world - stdout - RAM !\n");
		getchar();

		fprintf(stderr, "Hello world - stderr - RAM !\n");
		getchar();

		printf_P(PSTR("Hello world - ROM !\n"));
		getchar();

		fprintf_P(stdout, PSTR("Hello world - stdout - ROM !\n"));
		getchar();

		fprintf_P(stderr, PSTR("Hello world - stderr - ROM !\n"));
		getchar();
	}
}

/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baudrate generator value.
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

/*!
 * \brief Reads the character from serial port (UART0).
 *
 * \return received character.
 */
int ser_getch(FILE *stream)
{
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );

	/* get and return received data from buffer */
	return UDR0;
}

