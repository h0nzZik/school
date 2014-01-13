/**
 * @file main.c
 * @brief Simple memory tester
 * @author Jan Tusil <410062@mail.muni.cz>
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: sram.c,v 1.1 2006/12/04 21:26:28 qceleda Exp $
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

#define KeyESC		27

/* global and extern variables */
const char ver_str_pgm[]  PROGMEM = "\rATmega128 onchip SRAM ver. 0.1.0 - "__DATE__", "__TIME__;

char main_menu[] = "\n"
	"+---=[ SRAM tester ]=------------------------------------------+\n"
	"|                                                              |\n"
	"|  Simple memory tester                                        |\n"
	"|                                                              |\n"
	"+ command -+- function ----------------------------------------+\n"
	"|    b     | Set base address of memory window                 |\n"
	"|    n     | Set size of memory window (in bytes)              |\n"
	"|    r     | Run memory test on selected window                |\n"
	"+--------------------------------------------------------------+\n";

/* function prototypes */
void dumpsram(void);
void fillsram(void);
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);


/*
 * Memory tester
 */
struct MemoryTester
{
	// Stdin, stdout, stderr points here
	FILE inouterr;

	// Base address of memory window to test
	void *window_base;

	// Size of memory window to test (in bytes)
	size_t window_size;

	// Whether the test failed
	bool failed;

	// Whether the test failed during this frame
	bool failed_frame;
};

/**
 * @brief Initializes MemoryTester
 * @param mt pointer to MemoryTester instance
 */
void MemoryTester_init(struct MemoryTester *mt)
{
	/* open UART0 - 9600,N,8,1 */
	ser_init(UART_BAUD_SELECT(9600, F_CPU));
//	mt->inouterr = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);
	FILE f = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);
	mt->inouterr = f;

	mt->window_base = NULL;
	mt->window_size = 0;
	stdin = stdout = stderr = &mt->inouterr;
	puts_P(ver_str_pgm);
}

// This program consists of one instance of MemoryTester
struct MemoryTester app;

/**
 * @brief Asks user for address of memory window to test
 * @param self initialzed MemoryTester instance
 */
void MemoryTester_setWindowAddr(struct MemoryTester *self)
{
	printf("Enter window address: ");
	while (1 != scanf("%p", &self->window_base))
	{
		self->window_base = NULL;
		printf("Bad input. Try it again: ");
	}
}

/**
 * @brief Asks user for size of memory window to test
 * @param self initialized MemoryTester instance
 */
void MemoryTester_setWindowSize(struct MemoryTester *self)
{
	printf("Enter window size: ");
	while (1 != scanf("%zu", &self->window_size))
	{
		self->window_size = 0;
		printf("Bad input. Try it again: ");
	}
}

/**
 * @brief Called when memory testing fails
 * One can simply extend MemoryTester by modifying this function
 */
void MemoryTester_fail(struct MemoryTester *self)
{
	self->failed = true;
	self->failed_frame = true;
}


/**
 * @brief Tests memory using Moving Inversion algorithm
 */
void MemoryTester_run(struct MemoryTester *self)
{
	if ((self->window_base == NULL) || (self->window_size == 0))
	{
		printf("You should set window base address and size first\n");
		return;
	}


	// Moving inversion algorithm
	const uint8_t pattern = 0xA5;
	uint8_t *ptr = self->window_base;
	size_t i;

	// Fill memory with pattern
	memset((void *)self->window_base, pattern, self->window_size);

	self->failed = false;
	self->failed_frame = false;

	printf("Moving inversion: first round (low to high)\n");
	printf("Each square represents one 16B frame\n");
	printf("   0123456789ABCDEF"); // '\n' is printed in loop

	// Check whether it is filled. Then use inverted pattern
	for (i = 0; i < self->window_size; i++, ptr++)
	{
		if (*ptr != pattern)
		{
			MemoryTester_fail(self);
		}

		*ptr = ~pattern;

		if (i % 256 == 0)
		{
				printf("\n%04zx", i / 255);
		}

		// Draw a map of memory - square
		if (i % 16 == 15)
		{
				putchar(self->failed_frame ? 'X' : '#');
				self->failed_frame = false;
		}
	}

	printf("Moving inversion: second round (high to low)\n");
	printf("Only buggy addresses are printed\n");

	// Check whether memory is filled with inverted pattern.
	// Start from highest address
	while (ptr-- != self->window_base)
	{
			if (*ptr != ~pattern)
			{
					printf("Bad address: %p\n", ptr);
					MemoryTester_fail(self);
			}

			// For unknown reason
			*ptr = pattern;
	}

	printf("--------------------------------------");
	if (self->failed)
	{
		printf("Test failed\n");
	} else {
			printf("Test passed\n");
	}

}

int main( void )
{
	MemoryTester_init(&app);

	while(1)
	{
		puts(main_menu);

		switch( getchar() )
		{
			case 'b': MemoryTester_setWindowAddr(&app); break;
			case 'n': MemoryTester_setWindowSize(&app); break;
			case 'r': MemoryTester_run(&app); break;
			default: break;
		}
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
	(void)stream;
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );

	/* get and return received data from buffer */
	return UDR0;
}

