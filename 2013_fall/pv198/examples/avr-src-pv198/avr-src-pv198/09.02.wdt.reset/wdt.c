/*
 *                ATmega128 HW/SW reset example
 *
 * Description : HW/SW reset of the ATmega128 MCU. HW reset is forced by
 *               watchdog timer. SW reset is just jump to 0x0000
 *               address, no peripheries setup is performed.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: wdt.c,v 1.1 2006/12/10 12:20:27 qceleda Exp $
 */
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/wdt.h>

#define HW_RESET	0	/* hw reset (watchdog timer) */
#define SW_RESET	1	/* sw reset (jump to 0x0000) */

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* global variables */
#if 1			/* boot counter - correct approach */
	uint8_t bootctr __attribute__ ((section (".noinit")));
#else			/* boot counter - wrong approach */
	uint8_t bootctr;
#endif

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);
void reboot(uint8_t type);

int main(void)
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);
	uint8_t key;		/* pressed key */

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

	if(MCUCSR & _BV(PORF) || MCUCSR & _BV(JTRF))
		bootctr = 0;	/* clear boot counter after power-on reset */

	printf("Boot counter %d\n\n", bootctr++);

	MCUCSR = 0x00;	/* clear information which reset source caused an MCU reset */

	printf( "Press 's' for SW reset or 'h' for HW reset.\n\n");

	while(1)
	{
		key = toupper(getchar());	/* read a key */

		DDRD = 0xFF;			/* set port D as output */
		PORTD = 0x00;			/* turn on LEDs on port D */

		printf("Key pressed -> LEDs will be turned ON.\n");

		switch(key)
		{
			case 'H' :
				reboot(HW_RESET);	/* force HW reset */
				break;
			case 'S' :
				reboot(SW_RESET);	/* force SW reset */
				break;
			default:
				break;
		}
	}
}

/*!
 * \brief ATmega128 MCU reset.
 *
 * \param type  type of HW/SW reset.
 */
void reboot(uint8_t type)
{
	void (*reset) ();		/* pointer to reset function */

	if(type == HW_RESET)
	{
		wdt_enable(WDTO_15MS);	/* enable watchdog timer */
		while(1){};		/* wait for reset */
	}
	else
	{
		reset = 0x0000;         /* set function address */
		reset();                /* call reset function */
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

