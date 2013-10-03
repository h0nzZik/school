/*
 *                ATmega128 Buffer Overflow example
 *
 * Description : Stack buffer overflow is realized. Enable/disable
 *               assert macro to detect the buffer overflow.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: assert.c,v 1.1 2006/12/10 14:45:40 qceleda Exp $
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/wdt.h>

#if 1				/* disable/enable assert macro */
#define NDEBUG
#endif
#define __ASSERT_USE_STDERR	/* standard error output stream for assert */
#include <assert.h>		/* must be included after all assert's defines*/

#define DEST_SIZE	5	/* the size of destination string */

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);
void copy_string(char *s);

int main(void)
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stderr = stdin = &mystdout;

				/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	printf("\r\n---== Buffer overflow ==---\r\n\n");

	printf("The memory allocated for destination string is %d B.\n\n", DEST_SIZE);

	while(1)
	{
		printf("Press any key to copy first string\n");
		getchar();
		copy_string("1234");

		printf("Press any key to copy second string\n");
		getchar();
		copy_string("1234567");

		printf("Press any key to copy third string\n");
		getchar();
		copy_string("12345678901234567890");
	}
}

/*!
 * \brief Copy one string to another one.
 *
 * \param s  string with undefined size.
 */
void copy_string(char *src)
{
	char dest[DEST_SIZE];		/* small destination string */

	printf("\nSRC  string - \"%s\" - size %d\n", src, strlen(src));

	assert(strlen(src) < DEST_SIZE);

	strcpy(dest, src);

	printf("DEST string - \"%s\" - size %d\n\n", dest, strlen(dest));
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
/*!
 * \brief Reads the character from serial port (UART0).
 *
 * \return received character.
 */
int ser_getch(FILE *stream)
{
	/* wait for data to be received */
	while ( !(UCSR0A & _BV(RXC0)) );

	/* get and return received data from buffer */
	return UDR0;
}

