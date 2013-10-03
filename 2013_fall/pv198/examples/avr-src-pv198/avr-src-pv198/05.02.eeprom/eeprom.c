/*
 *                 ATmega128 onchip EEPROM example
 *
 * Description : Simple demo how to access on chip EEPROM.
 *
 * Note: 1) STK500 dongle from Charon 2 Development kit doesn't support
 *          EEPROM programming.
 *       2) OCDEN fuse must be set to enable single-byte EEPROM updates
 *          during ISP programming.
 *       3) Disable preserve EEPROM memory through the chip erase cycle
 *          (EESAVE=0).
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: eeprom.c,v 1.1 2006/11/28 23:03:02 celeda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
#define EE_SIZE     	4096                 /* eeprom size 4kB */

#define KeyESC		27

/* global and extern variables */
char PROGMEM ver_str_pgm[] = "\rATmega128 onchip EEPROM ver. 0.1.0 (flash) - "__DATE__", "__TIME__;
char EEMEM ver_str_ee[] = "\rATmega128 onchip EEPROM ver. 0.1.0 (eeprom) - "__DATE__", "__TIME__;

prog_char main_menu[] = "\n"
	"+---=[ EEPROM Demo Application ]=------------------------------+\n"
	"|                                                              |\n"
	"|  Simple demo how to access ATmega128 on chip EEPROM (4 kB).  |\n"
	"|                                                              |\n"
	"+ command -+- function ----------------------------------------+\n"
	"|    d     | Dump eeprom contents.                             |\n"
	"|    f     | Fill eeprom memory with pattern - normal mode.    |\n"
	"|    s     | Fill eeprom memory with pattern - smart mode.     |\n"
	"|    e     | Erase eeprom memory.                              |\n"
	"+--------------------------------------------------------------+\n";

/* function prototypes */
void dumpeeprom(void);
void filleeprom(void);
void sfilleeprom(void);
void eraseeeprom(void);
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
		puts_P(main_menu);

		switch( getchar() )
		{
			case 'd': dumpeeprom(); break;
			case 'f': filleeprom(); break;
			case 's': sfilleeprom(); break;
			case 'e': eraseeeprom(); break;
			default: break;
		}
	}
}

/*! \brief Fill eeprom memory with defined pattern.
 */
void filleeprom(void)
{
	uint8_t i = 0;
	uint8_t *addr; /* in AVR-GCC, all pointers are 16-bit */

	addr = 0;

	for(i=0; i != 0xff; i++)
	{
		eeprom_write_byte(addr++,i);

		if( !((uint16_t)addr%16) ) putchar('#');
	}

	putchar('\n');
}

/*! \brief Smart writing of eeprom memory with defined pattern.
 */
void sfilleeprom(void)
{
	uint8_t i = 0;
	uint8_t *addr; /* in AVR-GCC, all pointers are 16-bit */

	addr = 0;

	for(i=0; i != 0xff; i++)
	{
		if(i != eeprom_read_byte(addr))
			eeprom_write_byte(addr,i);

		addr++;

		if( !((uint16_t)addr%16) ) putchar('#');
	}

	putchar('\n');
}

/*! \brief Smart erasing of eeprom memory with defined pattern.
 */
void eraseeeprom(void)
{
	uint8_t i = 0;
	uint8_t *addr; /* in AVR-GCC, all pointers are 16-bit */

	addr = 0;

	for(i=0; i != 0xff; i++)
	{
		if(0xff != eeprom_read_byte(addr))
			eeprom_write_byte(addr,0xff);

		addr++;

		if( !((uint16_t)addr%16) ) putchar('#');
	}

	putchar('\n');
}

/*! \brief Display eeprom memory contents.
 */
void dumpeeprom(void)
{
	uint8_t i, j = 0, val, key = 0;
	uint8_t *addr; /* in AVR-GCC, all pointers are 16-bit */

	addr = 0;

	do
	{
		printf("0x%04x  ", (uint16_t) addr);/* display flash address */

		for(i=0; i<16; i++)		/* HEX dump */
		{
			printf("%02X ", eeprom_read_byte(addr++));
		}

		addr -=16;

		for(i=0; i<16; i++)		/* ASCII dump */
		{
			val = eeprom_read_byte(addr++);

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

