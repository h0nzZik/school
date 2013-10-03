/*
 *                 ATmega128 onchip SRAM example
 *
 * Description : Simple demo how to access on chip SRAM.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: sram.c,v 1.1 2006/12/04 21:26:28 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

#define KeyESC		27

/* global and extern variables */
prog_char ver_str_pgm[] = "\rATmega128 onchip SRAM ver. 0.1.0 - "__DATE__", "__TIME__;

char main_menu[] = "\n"
	"+---=[ SRAM Demo Application ]=--------------------------------+\n"
	"|                                                              |\n"
	"|  Simple demo how to access ATmega128 on chip SRAM (4 kB).    |\n"
	"|                                                              |\n"
	"+ command -+- function ----------------------------------------+\n"
	"|    d     | Dump SRAM contents.                               |\n"
	"|    f     | Fill SRAM with pattern.                           |\n"
	"+--------------------------------------------------------------+\n";

/* function prototypes */
void dumpsram(void);
void fillsram(void);
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);

int main( void )
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stdin = stderr =  &mystdout;

					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	puts_P(ver_str_pgm);

	while(1)
	{
		puts(main_menu);

		switch( getchar() )
		{
			case 'd': dumpsram(); break;
			case 'f': fillsram(); break;
			default: break;
		}
	}
}

/*! \brief Fill sram memory with defined pattern.
 */
void fillsram(void)
{
	uint16_t i = 0;
	uint8_t *addr; /* in AVR-GCC, all pointers are 16-bit */

	addr = (uint8_t *) 0x600;

	for(i=0; i < 0x100; i++)
	{
		*addr++ = i;

		if( !((uint16_t)addr%16) ) putchar('#');
	}

	putchar('\n');
}

/*! \brief Display sram memory contents.
 */
void dumpsram(void)
{
	uint8_t i, j = 0, val, key = 0;
	uint8_t *addr; /* in AVR-GCC, all pointers are 16-bit */

	addr = 0;
	
	do
	{
		printf("0x%04X  ", (uint16_t) addr);/* display flash address */

		for(i=0; i<16; i++)		/* HEX dump */
		{
			printf("%02X ", *addr++);
		}

		addr -=16;

		for(i=0; i<16; i++)		/* ASCII dump */
		{
			val = *addr++;

			if( val > 31 ) putchar(val);
			else putchar('.');
		}

		putchar('\n');

		j++;

		if( !(j%24) )			/* display 24 lines and prompt for keyboard input */
		{
			printf("Press any key to continue.");
			key = getchar();
			putchar('\n');
		}

	} while( key != KeyESC);
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

