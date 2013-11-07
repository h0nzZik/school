/*
 *                  R S 2 3 2 - s t r i n g - e x a m p l e
 *
 * Description : Simple demo how to write RAM and ROM based strings
 *               to ATmega128's serial port (UART0).
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.3 2008/11/07 08:31:44 qceleda Exp $
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init( uint16_t baud );
void ser_puts( char *s );
void ser_puts_P( prog_char *s );
uint8_t ser_getch( void );

typedef struct
{
	bool on;
	int8_t temp;
} SHeatingSystem;

SHeatingSystem system;

typedef struct
{
	char *cmd;
	bool (*f)(SHeatingSystem *self, const char *arg);
} CmdMap_t;


bool SHeatingSystem_restart(SHeatingSystem *self, const char *arg)
{
	ser_puts("\033[2J");
	self->on=false;
	self->temp=20;
	return true;
}

bool SHeatingSystem_set_temperature(SHeatingSystem *self, const char *arg)
{
	int t;
	if (!arg || 1 != sscanf(arg,"%d", &t))
	{
		return false;
	}

	if (t < -20 || t > 50)
	{
		return false;
	}

	self->temp = t;

	return true;
}


bool SHeatingSystem_on(SHeatingSystem *self, const char *arg)
{
	self->on=1;
	return true;
}

bool SHeatingSystem_off(SHeatingSystem *self, const char *arg)
{
	self->on=0;
	return true;
}

bool SHeatingSystem_status(SHeatingSystem *self, const char *arg)
{
	if (self->on)
	{
		ser_puts_P(PSTR("Heating system is ON\r\n"));

		char buf[10];
		sprintf(buf, "%d", self->temp);
		ser_puts_P(PSTR("Temperature: "));
		ser_puts(buf);
		ser_puts_P(PSTR(" °C\r\n"));
	}
	else
	{
		ser_puts_P(PSTR("Heating system is OFF\r\n"));
	}


	return true;
}

static const CmdMap_t command_mapping[] =
{
	{
		"on",
		SHeatingSystem_on,
	},
	{
		"off",
		SHeatingSystem_on,
	},
	{
		"status",
		SHeatingSystem_status,
	},
	{
		"restart",
		SHeatingSystem_restart,
	},
	{
		"set_temperature",
		SHeatingSystem_set_temperature,
	}
};

static const uint8_t n_commands = sizeof(command_mapping) / sizeof(CmdMap_t);


void print_commands(void)
{
	uint8_t i;

	ser_puts_P(PSTR("Available commands: \r\n"));
	for (i=0; i<n_commands; i++)
	{
		ser_puts(command_mapping[i].cmd);
		ser_puts("\r\n");
	}
}

void read_line(char *buf, uint8_t max_bytes)
{
	uint8_t i;
	bool done = false;

	while(!done)
	{
		ser_puts_P(PSTR("heating system # "));
		for (i=0; i<max_bytes; i++)
		{
			// Read non-control character
			char c;
			do {
				c = ser_getch();
			} while (iscntrl(c) && (c != '\n') && (c != '\r'));

			if (c == '\n' || c == '\r')
			{
				ser_puts("\r\n");
				done = true;
				buf[i] = '\0';
				break;
			}

			ser_putch(c);
			buf[i] = c;
		}

		if (done)
		{
			return;
		}

		ser_puts_P(PSTR("\r\nerror: line is too long"));

		// Wait for end of line
		while( '\n' != ser_getch())
		{
			;
		}
	}
}

int strcmp_towhite(const char *s1, const char *s2)
{
	while (*s1 && *s2 && !isspace(*s1) && !isspace(*s2) && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	if ((isspace(*s1) || !*s1) && (isspace(*s2) || !*s2))
	{
		return 0;
	}

	return *s2 - *s1;
}

void main_loop_iter(void)
{
	char buf[40];


	read_line(buf, 40);
	buf[39]=0;


	// Ignore spaces at the beginning
	char *s = buf;
	while(*s && isspace(*s))
	{
		s++;
	}

	// Ignore empty commands
	if (!*s)
	{
		return;
	}

	// Try to find parameters
	char *arg = s;
	while (*arg && !isspace(*arg))
	{
		arg++;
	}

	if (!*arg)
	{
		arg = NULL;
	}

	// Try to find corresponding command
	uint8_t i;
	bool (*f)(SHeatingSystem *self, const char *arg) = NULL;

	for (i=0; i<n_commands; i++)
	{
		if (!strcmp_towhite(s, command_mapping[i].cmd))
		{
			ser_puts("found command: ");
			ser_puts(command_mapping[i].cmd);
			ser_puts("\r\n");

			f = command_mapping[i].f;
			break;
		}

	}

	if (!f)
	{
		ser_puts("\033[31mNo such command:\033[39m '");
		ser_puts(s);
		ser_puts("'\r\n");
		return;
	}

	bool rv = f(&system, arg);
	if (!rv)
	{
		ser_puts("\033[31mCommand failed\033[39m\r\n");
		return;
	}
}

int main( void )
{
	static prog_char hello_str[] = "Hello world - ROM string - II !\r\n";

					/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );


	SHeatingSystem_restart(&system, NULL);
	print_commands();
	while(1)
	{
		main_loop_iter();
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



void ser_putch(char c)
{

	/* wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) );

	/* put data into buffer and sends the data */
	UDR0 = c;

}

/*!
 * \brief Writes the string s to serial port (UART0).
 *
 * \param s  string to write.
 */
void ser_puts(char *s)
{
	while (*s)
	{
		/* wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE)) );

		/* put data into buffer and sends the data */
		UDR0 = *s++;
	}
}

/*!
 * \brief Writes the ROM string s to serial port (UART0).
 *
 * \param s  ROM placed string to write.
 */
void ser_puts_P(prog_char *s)
{
	char c = (char) pgm_read_byte (s);

	while(c)
	{
		/* wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE)) );

		/* put data into buffer and sends the data */
		UDR0 = c;

		/* read character from program memory */
		c = (char) pgm_read_byte (++s);
	}
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

