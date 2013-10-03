/*
 *                  F L A S H - crc and flash dump - e x a m p l e
 *
 * Description : Calculate flash image XMODEM 16 bit CRC and write flash
 *               contents to serial port.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: flash.c,v 1.3 2006/11/28 22:24:45 celeda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* global and extern variables */
prog_char ver_str[] = "\rATmega128 Flash Memory Checksum Tester ver. 0.1.0 - "__DATE__", "__TIME__"\n";

extern uint16_t __data_load_end;		/* address of last program byte */

/* function prototypes */
int flashcrc(void);
void dumpflash(void);
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);

int main( void )
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stdin = stderr =  &mystdout;

					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	puts_P(ver_str);

	printf("\nFlash contents is %s.\n\n", flashcrc() ? "broken" : "ok");

	while(1)
	{
		dumpflash();
	}
}

/*! \brief Display flash memory contents.
 */
void dumpflash(void)
{
	uint16_t addr=0;
	uint8_t i,j=0, val;

	while(1)
	{
		printf("0x%04x  ", addr);	/* display flash address */

		for(i=0; i<16; i++)		/* HEX dump */
		{
			printf("%02X ", pgm_read_byte(addr++));
		}

		addr -=16;

		for(i=0; i<16; i++)		/* ASCII dump */
		{
			val = pgm_read_byte(addr++);
			if( val > 31 ) putchar(val);
			else putchar('.');
		}

		putchar('\n');

		j++;

		if( !(j%24) )			/* display 24 lines and prompt for keyboard input */
		{
			printf("Press any key to continue.");
			getchar();
			putchar('\n');
		}
	}
}

/*! \brief Calculate flash image XMODEM 16 bit CRC value
 *
 * \note flashcrc function doesn't support flash parts > 64 kB.
 *
 * \return 0 on success, -1 on error
 */
int flashcrc(void)
{
	uint16_t i,
		crc = 0,
		crc_addr;
	uint8_t  val;

	crc_addr = (uint16_t) &__data_load_end;    /* get address of last program byte */

	for (i = 0; i < crc_addr; i++)		/* calculate flash CRC */
	{
		val = pgm_read_byte(i);
		crc = _crc_xmodem_update(crc, val);
	}

	printf("CRC address HEX - 0x%04X, DEC - %d\n", crc_addr, crc_addr);
	printf("Flash CRC value 0x%04X\n", pgm_read_word(crc_addr));
	printf("Calc. CRC value 0x%04X\n", crc);

	if (crc != pgm_read_word(crc_addr))
		return -1;			/* flash contents is broken */
	else
		return 0;			/* flash contents is ok */
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

