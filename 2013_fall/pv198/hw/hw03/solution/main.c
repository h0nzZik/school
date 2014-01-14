/**
 * @file main.c
 * @brief Heating system homework.
 * @author Jan Tusil <410062@mail.muni.cz>
 * 
 *
 * Description: Acts like heating system, but in fact does nothing.
 *              Use serial port (UART0) to communicate.
 *
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)


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


/**
 * @brief Restarts heating system
 */
bool SHeatingSystem_restart(SHeatingSystem *self, const char *arg)
{
	ser_puts("\033[2J");
	self->on=false;
	self->temp=20;
	return true;
}

/**
 * @brief Sets reqired temperature
 */
bool SHeatingSystem_set_temperature(SHeatingSystem *self, const char *arg)
{
	int t;
	if (!arg || 1 != sscanf(arg,"%d", &t))
	{
		return false;
	}

	if (t < -20 || t > 50)
	{
		printf("This temperature is out of supported range\n");
		return false;
	}

	self->temp = t;

	return true;
}


/**
 * @brief Turns heating system on
 */
bool SHeatingSystem_on(SHeatingSystem *self, const char *arg)
{
	self->on=1;
	return true;
}

/**
 * @brief Turns heating system off
 */
bool SHeatingSystem_off(SHeatingSystem *self, const char *arg)
{
	self->on=0;
	return true;
}


/**
 * @brief Prints status of heating system
 */
bool SHeatingSystem_status(SHeatingSystem *self, const char *arg)
{
	if (self->on)
	{
		ser_puts_P(PSTR("Heating system is ON\r\n"));

		char buf[10];
		sprintf(buf, "%d", self->temp + rand() % 3 - 1);
		ser_puts_P(PSTR("Real temperature: "));
		ser_puts(buf);
		ser_puts_P(PSTR(" °C\r\n"));

		printf("Wanted temperature: %d °C\r\n", self->temp);
	}
	else
	{
		ser_puts_P(PSTR("Heating system is OFF\r\n"));
	}


	return true;
}

/**
 * @brief Wrapper for print_commands()
 */
bool SHeatingSystem_print_commands(SHeatingSystem *self, const char *arg)
{
	void print_commands(void);
	print_commands();
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
	},
	{
		"help",
		SHeatingSystem_print_commands,
	}
};

static const uint8_t n_commands = sizeof(command_mapping) / sizeof(CmdMap_t);


/**
 * @brief Prints available commands
 */
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

/**
 * @brief Prints prompt and requests a line from user
 * @param buf Buffer to store line
 * @param max_bytes How many bytes store into buffer at max.
 */
void read_line(char *buf, uint8_t max_bytes)
{
	uint8_t i;
	bool done = false;

	while(!done)
	{
		ser_puts_P(PSTR("\033[33mheating system # \033[39m"));
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

/**
 * @brief Compares strings to first occurence of whitespace
 * Acts like strcmp(), but whitespace has the same meaning as terminating zero
 * @param s1 first string
 * @param s2 second string
 */
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

/**
 * @brief One iteration of main loop
 */
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
	/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );
	FILE f;
	fdev_setup_stream(&f, (void*)ser_putch, (void*)ser_getch, _FDEV_SETUP_RW);
	stdin = stdout = stderr = &f;


	SHeatingSystem_restart(&system, NULL);
	printf("\r\nWelcome to HeatingSystem v0.1\r\n");
	print_commands();
	while(1)
	{
		main_loop_iter();
	}
}


