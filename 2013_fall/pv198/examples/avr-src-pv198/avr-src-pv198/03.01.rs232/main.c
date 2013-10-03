/*
 *                  R S 2 3 2 - putch/getch - e x a m p l e
 *
 * Description : Elementary functions used for 8-bit serial communication
 *               between ATmega128 and other systems.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.4 2008/11/07 08:28:38 qceleda Exp $
 */
#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init( uint16_t baud );
void ser_putch( uint8_t c );
uint8_t ser_getch( void );

int main( void )
{
	uint8_t i;
					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	while(1)
	{
		i = 32;			/* skip non-printable characters */

		do
		{			/* print ASCII table */
			ser_putch( i++ );

			if( !(i%32) )	/* add new line */
			{
				ser_putch('\r'); /* carriage return */
				ser_putch('\n'); /* line feed */
			}

		} while(i);

		ser_getch();		/* wait until key pressed */
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
void ser_putch(uint8_t c)
{
	/* wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) );

	/* put data into buffer and sends the data */
	UDR0 = c;
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



