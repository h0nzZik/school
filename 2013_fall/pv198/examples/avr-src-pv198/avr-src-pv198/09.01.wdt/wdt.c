/*
 *                ATmega128 Watchdog Timer example
 *
 * Description : The different ATmega128 reset sources are shown. The
 *               watchdog timer can be enabled, cleared or disabled.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: wdt.c,v 1.1 2006/12/10 09:43:39 qceleda Exp $
 */
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/wdt.h>

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);

int main(void)
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stderr = stdin = &mystdout;

				/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	printf("\r\n---== Watchdog Timer ==---\r\n\n");

	printf("MCU reset flags - 0x%02x\n\n", MCUCSR);

	printf(" Power-On Reset   %s\n", MCUCSR & _BV(PORF) ? "1" : "0");
	printf(" External Reset   %s\n", MCUCSR & _BV(EXTRF) ? "1" : "0");
	printf(" Brown-out Reset  %s\n", MCUCSR & _BV(BORF) ? "1" : "0");
	printf(" Watchdog Reset   %s\n", MCUCSR & _BV(WDRF) ? "1" : "0");
	printf(" JTAG Reset       %s\n\n", MCUCSR & _BV(JTRF) ? "1" : "0");

	MCUCSR = 0x00;	/* clear information which reset source caused an MCU reset */

	printf( "Press 'd' to disable, 'e' to enable or any key to clear " \
		"the watchdog timer.\n\n");

	while(1)
	{
		switch(toupper(getchar()))
		{
			case 'E' : wdt_enable(WDTO_2S);	/* enable watchdog timer */
				printf("Watchdog enabled.\n");
				break;
			case 'D' : wdt_disable();	/* disable watchdog timer */
				printf("Watchdog disabled.\n");
				break;
			default:
				wdt_reset();		/* reset watchdog timer */
				printf("Watchdog cleared.\n");
				break;
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

