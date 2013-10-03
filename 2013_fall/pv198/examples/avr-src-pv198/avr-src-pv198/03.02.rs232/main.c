/*
 *                  R S 2 3 2 - s t r i n g - e x a m p l e
 *
 * Description : Simple demo how to write RAM and ROM based strings
 *               to ATmega128's serial port (UART0).
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.3 2008/11/07 08:31:44 qceleda Exp $
 */
#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init( uint16_t baud );
void ser_puts( char *s );
void ser_puts_P( prog_char *s );
uint8_t ser_getch( void );

int main( void )
{
	static prog_char hello_str[] = "Hello world - ROM string - II !\r\n";

					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	while(1)
	{
		ser_puts("Hello world - RAM string !\r\n");
		ser_getch();		/* press any key */

		ser_puts_P(PSTR("Hello world - ROM string - I !\r\n"));
		ser_getch();		/* press any key */

		ser_puts_P(hello_str);
		ser_getch();		/* press any key */
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
 * \brief Writes the string s to serial port (UART0).
 *
 * \param s  string to write.
 */
void ser_puts(char *s)
{
	while (*s)
	{
		/* wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE)) );

		/* put data into buffer and sends the data */
		UDR0 = *s++;
	}
}

/*!
 * \brief Writes the ROM string s to serial port (UART0).
 *
 * \param s  ROM placed string to write.
 */
void ser_puts_P(prog_char *s)
{
	char c = (char) pgm_read_byte (s);

	while(c)
	{
		/* wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE)) );

		/* put data into buffer and sends the data */
		UDR0 = c;

		/* read character from program memory */
		c = (char) pgm_read_byte (++s);
	}
}

/*!
 * \brief Reads the character from serial port (UART0).
 *
 * \return received character.
 */
uint8_t ser_getch(void)
{
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );

	/* get and return received data from buffer */
	return UDR0;
}

