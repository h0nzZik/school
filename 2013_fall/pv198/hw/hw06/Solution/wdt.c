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

// C headers
#define __ASSERT_USE_STDERR 
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// AVR headers
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

// User headers
#include "term.h"

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)



/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);



/**
 * @brief Structure, representing FingerGame
 */
struct FingerGame
{
	FILE inout;

	/**
	 * @brief Current level
	 */
	int level;


	int fails;

	char username[10];
};

const char FingerGame_WelcomeScreen[] = "\n"
	"+---=[ Welcome to FingerGame ]=--------------------------------+\n"
	"|                                                              |\n"
	"|  FingerGame is simple application for learning,              |\n"
	"| how to use computer keyboard with all your fingers.          |\n"
	"| To start a game, simple press any key and start typing,      |\n"
	"| what you see in first line.                                  |\n"
	"|                                                              |\n"
	"+--------------------------------------------------------------+\n";


/**
 * @brief How many characters are pre level
 */
const int FingerGame_chars_per_level = 40;

/**
 * @brief Structure, representing one level of FingerGame
 */
struct FingerGameLevel
{
	uint8_t wdt_value;
	bool enable_case_sensitivity;
	bool enable_numbers;
	uint8_t max_fails_per_level;
};

/**
 * @brief Some basic levels of FingerGame.
 */
struct FingerGameLevel FingerGame_levels[] = {
	{ WDTO_2S, false, false, 10},
	{ WDTO_2S, true, false, 8},
	{ WDTO_1S, false, false, 8},
	{ WDTO_1S, false, false, 5},
	{ WDTO_500MS, false, false, 5}
};

/**
 * @brief How many levels we have?
 */
const int FingerGame_n_levels = sizeof(FingerGame_levels) / sizeof(struct FingerGameLevel);


/**
 * @brief Let user type his username.
 */
void FingerGame_Username(struct FingerGame *self)
{
	printf("Please, type your username here: _________");
	Term_CursorLeft(stdout, 9);
	int i = 0;
	while(1)
	{
		int c = getchar();

		if (c == '\n' || c == '\r')
		{
			self->username[i] = '\0';
			break;
		}

		if (c == '\b' || c == 0x7F)
		{
			if (i != 0)
			{
				Term_Back(stdout, '_');
				i--;
				self->username[i] = '0';
			}
			continue;
		}

		if (!isprint(c))
			continue;

		// Last position
		if (i == 9)
		{
			i--;
		}

		self->username[i] = c;
		putchar(c);
		i++;

	}

	printf("\nHi ");
	Term_FgColorSet(stdout, TERM_COLOR_YELLOW);
	printf("%s\n", self->username);
	Term_FgColorClear(stdout);

}

/**
 * @brief Initializes FingerGame.
 * Setups UART and std(in|out|err)
 */
void FingerGame_initSystem(struct FingerGame *self)
{
	fdev_setup_stream(&self->inout, ser_putch, ser_getch, _FDEV_SETUP_RW);
	stdout = stderr = stdin = &self->inout;

	/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );
}

void FingerGame_initGame(struct FingerGame *self)
{
	self->level = 0;
}

/**
 * @brief Prints information about current level
 */
void FingerGame_LevelInfo(struct FingerGame *self)
{
	printf("-----< Level %2d >-----\n", self->level);
	printf("Big letters: %s\n", FingerGame_levels[self->level].enable_case_sensitivity ? "enabled" : "disabled");
	printf("Numbers:     %s\n", FingerGame_levels[self->level].enable_numbers ? "enabled" : "disabled");
	printf("Errors:      %d\n", FingerGame_levels[self->level].max_fails_per_level);
	printf("Time limit:  ");
	

	switch(FingerGame_levels[self->level].wdt_value)
	{
		case WDTO_15MS:
			printf("15 ms\n");
			break;

		case WDTO_30MS:
			printf("30 ms\n");
			break;

		case WDTO_60MS:
			printf("60 ms\n");
			break;

		case WDTO_120MS:
			printf("120 ms\n");
			break;

		case WDTO_500MS:
			printf("500 ms\n");
			break;

		case WDTO_1S:
			printf("1 s\n");
			break;

		case WDTO_2S:
			printf("2 s\n");
			break;

		default:
			printf("oops.\n");
			break;
	}
}

/**
 * @brief Try next level or win
 */
void FingerGame_LevelDone(struct FingerGame *self)
{
	printf("\nLevel done\n");
	self->level++;
	if (self->level == FingerGame_n_levels)
	{
		printf("You have finished whole game. Contratulations!\n");
		printf("Press any key to play again\n");

		// TODO: Log high scores

		getchar();
		wdt_enable(WDTO_15MS);
		do {
			putchar('.');
		}while(1);
	}
}

/**
 * @brief Generates random character.
 * Set of characters depends on level settings.
 * Each character has (approx.) same probability to be generated
 */
char FingerGame_RandChar(struct FingerGame *self)
{
	unsigned int mod = 0;
	mod += 'z' - 'a';
	if (FingerGame_levels[self->level].enable_case_sensitivity)
	{
		mod += 'Z' - 'A';
	}

	if (FingerGame_levels[self->level].enable_numbers)
	{
		mod += '9' - '0';
	}

	int r = rand() % mod;

	if (r < 'z' - 'a')
	{
		return 'a' + r;
	}

	r -= 'z' - 'a';

	if (r < 'Z' - 'A')
	{
		return 'A' + r;
	}

	r -= 'Z' - 'A';

	if (r < '9' - '0')
	{
		return '0' + r;
	}

	// Shold not happen
	
	assert("Problem generating numbers"[0] == 0);
	return '?';
}

/**
 * @brief He lost the game because is slow -> tell him it.
 */
void FingerGame_LoseStat(struct FingerGame *self)
{
	// He won, so do not display this ;-)
	if (self->level == FingerGame_n_levels)
		return;

	putchar('\n');
	Term_FgColorSet(stdout, TERM_COLOR_YELLOW);
	printf("%s", self->username);
	Term_FgColorClear(stdout);
	printf(",\nyou were too slow. But you can try it again :-)\n");
	printf("Of course I mean - try the ");

	Term_FgColorSet(stdout, TERM_COLOR_GREEN);
	printf("whole");
	Term_FgColorClear(stdout);
	printf(" game again :-P\n\n\n");
}
/**
 * @brief Prepares FingerGame for running selected level
 */
void FingerGame_LevelStart(struct FingerGame *self)
{
	wdt_disable();
	self->fails = 0;

	printf("Press space to start\n");
	do {
		;
	} while (' ' != getchar());
	wdt_enable(FingerGame_levels[self->level].wdt_value);
}

/**
 * @brief Runs selected FingerGame level
 */
bool FingerGame_LevelMain(struct FingerGame *self)
{
	FingerGame_LevelInfo(self);
	FingerGame_LevelStart(self);

	// Scroll one line
	printf("repeat: \n");
	Term_CursorDown(stdout, 1);
	printf("      > ");
	Term_CursorUp(stdout, 1);

	int i;
	for (i = 0; i<FingerGame_chars_per_level; i++)
	{
		// Original
		int orig = FingerGame_RandChar(self);
		Term_FgColorSet(stdout, TERM_COLOR_YELLOW);
		putchar(orig);
		Term_FgColorClear(stdout);
		Term_CursorLeft(stdout, 1);


		// User
		Term_CursorDown(stdout, 1);
		// Let him have all the time
		wdt_reset();
		int user = getchar();
		wdt_reset();


		if (user == orig)
		{
			putchar(user);	
		} else {
			// TODO: <red>
			Term_FgColorSet(stdout, TERM_COLOR_RED);
			putchar(user);
			Term_FgColorClear(stdout);

			self->fails++;
			if (self->fails > FingerGame_levels[self->level].max_fails_per_level)
			{
				return false;
			}
		}
		Term_CursorUp(stdout, 1);


	}

	return true;
}



struct FingerGame app __attribute__ ((section (".noinit")));

int main(void)
{
	bool wd_reset = !!(MCUCSR & _BV(WDRF));
	MCUSR = 0x00;

	// Watchdog reset or not?
	FingerGame_initSystem(&app);

	if (wd_reset)
	{
		FingerGame_LoseStat(&app);
	} else {
		// Show welcome screen
		puts(FingerGame_WelcomeScreen);
	}

	FingerGame_initGame(&app);

	// Enter username and generate random number
	TCCR0 = 0x01; // normal mode, clock from T0S
	FingerGame_Username(&app);
	TCCR0 = 0x00; // Stop timer.
	srand(TCNT0); // Initialize PRG using timer value

	while(1)
	{
		bool rv = FingerGame_LevelMain(&app);

		// If user completed level successfully
		if (rv == true)
		{
			FingerGame_LevelDone(&app);
		} else {
			printf("\nOops, too many errors. Try this level again\n");
		}

	}


	MCUCSR = 0x00;	/* clear information which reset source caused an MCU reset */

	return 0;
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

