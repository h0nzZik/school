/*
 *                  R S 2 3 2 - s t d i o - e x a m p l e
 *
 * Description : Simple demo how to use printf function to write
 *               data to ATmega128's serial port (UART0).
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.4 2008/11/07 08:32:37 qceleda Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

#define PI 3.141592

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);
void printf_fmt_output(void);

int main( void )
{
	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stdin = stderr =  &mystdout;

					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

					/* display program version */
	printf_P(PSTR("\rATmega128 RS232 - printf example - version 1.0 \n"
				"Build on %s, %s with AVR-GCC %d.%d\n\n"),
				__DATE__, __TIME__, __GNUC__, __GNUC_MINOR__);		

	while(1)
	{				/* printf format output demo */
		printf_fmt_output();
	}
}

/*! \brief Printf output demo.
 *
 * Displays different types of numeric formatted output used
 * in embedded control systems.
 */
void printf_fmt_output(void)
{
	int int_nr  = 12345, hex_nr  = 0x12ab;
	long long_nr = 1234567890;
	unsigned char uc_nr = 248;

	puts("We will display following numbers\n\n"
		"  int           - 12345\n"
		"  int           - 0x12ab\n"
		"  long          - 1234567890\n"
		"  unsigned char - 248\n");

	puts("1) unformatted output\n");
	printf(" %d\n %x\n %ld\n %u\n", int_nr, hex_nr, long_nr, uc_nr);

	puts("\n2) formatted output - right justify the output\n");
	printf(" %12d\n %12x\n %12ld\n %12u\n\n", int_nr, hex_nr, long_nr, uc_nr);

	puts("Press any key to continue ...");
	getchar();

	puts("\n3) formatted output - right justify the output, pad numbers with leading zeros\n");
	printf(" %010d\n %010x\n %010ld\n %010u\n", int_nr, hex_nr, long_nr, uc_nr);

	puts("\n4) formatted output - display a + sign on positive numbers\n");
	printf(" %+12d\n %+12ld\n", int_nr, long_nr);

	puts("\n5) formatted output - different case of hex numbers\n");
	printf(" %x\n %X\n",hex_nr, hex_nr);

	puts("\n6) formatted output - display hex number's prefix\n");
	printf(" %#x\n %#X\n\n", hex_nr, hex_nr);

	puts("Press any key to continue ...");
	getchar();

	puts("\n7) formatted output - display floating point numbers\n");
	printf(" %f\n", PI);
	printf(" %.2f          - rounded to 2 dig \n", PI);
	printf(" %e\n", PI);
	printf(" %.2e      - rounded to 2 dig \n\n", PI);

	puts("Press any key to continue ...");
	getchar();
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

