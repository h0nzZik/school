/*
 *                  R S 2 3 2 - Auto Baud Rate Detector
 *
 * Description : Auto baud rate detection based on start bit measurement.
 *               Press ENTER key to detect current terminal baud rate.
 *               Based on different sources from Internet (mainly by
 *               Nard Awater), tested with baud rates from 300 - 230400 Bd.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: autobaudrate.c,v 1.2 2008/11/07 08:36:47 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>

/* function prototypes */
void ser_init( uint16_t baud );
int ser_putch(char c, FILE *stream);
void reboot(void);

int main( void )
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, NULL, _FDEV_SETUP_RW);
	uint16_t ubrr;

	stdout = stderr =  &mystdout;

	/* Start bit measurement, press ENTER to generate right pattern
	 * for measurement (00001101)
	 *  ----+  +--+  +--+--+           +--+-------
	 *      |__|  |__|     |__.__.__.__|
	 *                                   ^
	 *       ^  ^                     ^  +--- stop bit(s)
	 *       |  +---------------------+------ data bits (LSB first)
	 *       +------------------------------- start bit
	 */

	TCCR1B = 0;	 		/* stop timer */
	TCNT1 = 0;			/* reset the counter */

	while (PINE & _BV(PORTE0));	/* wait for begin of the start bit */
	TCCR1B = (1<<CS10);		/* start timer (no prescaling) */

	while (!(PINE & _BV(PORTE0)));	/* wait for end of start bit */
	TCCR1B = 0;			/* stop timer */

	/*
	 * USART0 is used in asynchronous normal mode
	 *
	 * The baud rate generator is a down-counter, running at system clock
	 * (fosc). A clock is generated each time the counter reaches zero. This
	 * clock is the baud rate generator clock output (= fosc/(UBRR+1)). The
	 * transmitter divides the baud rate generator clock output by 2, 8, or 16
	 * depending on mode. For asynchronous normal mode
	 *
	 * BAUD = fosc/(16*(UBRR+1))
	 * UBRR = (fosc/(16*BAUD))-1
	 *
	 * With TCNT1 we will measure the length of start bit (number of system
	 * tics). This value divided by 2,8, or 16 depending on mode is equal to
	 * UBBR value.
	 */

	ubrr = TCNT1 >> 4;		/* measured UBRR value */

	ser_init( ubrr );		/* open UART0 - xxxx,N,8,1 */

	printf("\n--== Auto Baud Rate Detector ==--\n\n");
	printf("F_CPU =  %ld Hz\n", F_CPU);
	printf("UBRR  =  %d\n", ubrr);
	printf("BAUD  =  %ld Bd\n\n", F_CPU/(16*(ubrr+1)));
	printf("Press ENTER to test again baud rate detection.\n");

	reboot();			/* reboot AVR microcontroller */

	while(1) {};
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
	while ( !( UCSR0A & _BV(UDRE0)) );

	/* put data into buffer and sends the data */
	UDR0 = c;
	return 0;
}

/*!
 * \brief Restart AVR microcontroller, just jump to address 0.
 */
void reboot(void)
{
	void (*reset) ();	/* pointer to reset function */

	reset = 0x0000;		/* set function address */

	reset();		/* call reset function */
}

