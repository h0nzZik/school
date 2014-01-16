/**
 * @file wdt.c
 * @brief Implementation of simple finger-training game.
 * Use UART to play.
 * @author Jan Tusil <410062@mail.muni.cz>
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
#include <avr/interrupt.h>

#include "shift.h"

/* calculate baud rate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

					/* UART0 definitions */
#define UART0_PORT            PORTE
#define UART0_DDR             DDRE
#define UART0_PIN             PINE

#define UART0_RTS             PORTE3
#define UART0_CTS             PORTE2
#define UART0_TXD             PORTE1
#define UART0_RXD             PORTE0

#define UART0_RTS_DDR         DDE3
#define UART0_CTS_DDR         DDE2
#define UART0_TXD_DDR         DDE1
#define UART0_RXD_DDR         DDE0

#define HALT_ON_RESET 1

/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baudrate generator value.
 */
void ser_init(uint16_t baud, bool int_rx, bool int_tx)
{
	/* set baud rate */
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t) baud;

	/* enable receiver and transmitter */
	uint8_t tmp = _BV(RXEN0)|_BV(TXEN0);
	if (int_rx)
	{
		tmp |= _BV(RXCIE0);
	}

	if (int_tx)
	{
		tmp |= _BV(TXCIE0);
	}

	UCSR0B = tmp;

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

#if 1
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

#endif


inline void reset_computer_start()
{
	UART0_PORT &= ~(_BV(UART0_RTS));
}


inline void reset_computer_stop()
{
	UART0_PORT |= _BV(UART0_RTS);
}


/**
 * @brief Reads a byte from dips
 */
uint8_t DevBoardShiftByteIn( void )
{
	uint8_t in_byte=0, i=7;

	SHIFT_DDR |= _BV(SHIFT_IN_LOAD_DDR);	/* shift/!load set as output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* clock set as output pin */
	SHIFT_DDR &= ~(_BV(SHIFT_IN_DDR));	/* serial data set as input pin */

	SHIFT_PORT &= ~(_BV(SHIFT_CLK));	/* set clock low  */
	SHIFT_PORT &= ~(_BV(SHIFT_IN_LOAD));	/* set load low  -> parallel in */

	asm volatile("nop\n\t");		/* wait */
	SHIFT_PORT |= _BV(SHIFT_IN_LOAD);       /* set load high -> serial out */

	/* read bit, MSB(most significant bit) first */
	bit_is_set(SHIFT_PIN, SHIFT_IN) ? (in_byte |= 1) : (in_byte |= 0);
	while(i--)
	{
		SHIFT_PORT |= _BV(SHIFT_CLK);	/* 0 -> 1 next databit on serial output is valid */
		asm volatile("nop\n\t");	/* wait */
		in_byte <<= 1;			/* shift one bit left */
		bit_is_set(SHIFT_PIN, SHIFT_IN) ? (in_byte |= 1) : (in_byte |= 0); /* read next bit */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
		asm volatile("nop\n\t");	/* wait */
	}

	return(in_byte);			/* return received byte */
}


/**
 * @brief Structure, representing FingerGame
 */
struct SmartWDT
{
	FILE inout;

	enum {
		SWDT_MODE_INITIAL,
		SWDT_MODE_SETUP,
		SWDT_MODE_WATCH,
		SWDT_MODE_WATCH_DISABLED,
		SWDT_MODE_RESET,
		SWDT_MODE_BOOT,
	} mode;

	/**
	 * @brief How many second to wait
	 */
	uint8_t seconds;

	/**
	 * @brief Ticks in this second
	 */

	uint8_t tmr_ticks;

	uint8_t waiting_time;
};

static const uint8_t SmartWDT_TimerIntsPerSec = 225;

const char SmartWDT_WelcomeScreen[] = "\n"
	"+---=[ Welcome to SmartWDT ]=----------------------------------+\n"
	"|                                                              |\n"
	"| supported commands:                                          |\n"
	"|        '?' => prints welcome screen                          |\n"
    "|        '#' => clears the watchdog timer (if monitoring)      |\n"
    "|        'e' => disable monitoring (if enabled)                |\n"
    "|        'd' => enable monitoring (if disabled)                |\n"
	"|                                                              |\n"
	"+--------------------------------------------------------------+\n";


void SmartWDT_ClearTimer(struct SmartWDT *self)
{
	self->seconds = 0;
	self->tmr_ticks = 0;
	// We do not clear timer,
	// it causes an error < 50 ms.
}

/**
 * @brief handles 1s timer
 */
void SmartWDT_TickTimer(struct SmartWDT *self)
{
	switch(self->mode)
	{
		case SWDT_MODE_WATCH:
			putchar('-');
			self->seconds++;
			if (self->seconds >= self->waiting_time)
			{
				self->seconds = 0;
				self->mode = SWDT_MODE_RESET;
				printf("debug: Going to reset computer\n");
			}
			break;

		case SWDT_MODE_RESET:
			putchar('-');
			self->seconds++;
			if (self->seconds == 4)
			{
				self->seconds = 0;
				self->mode = SWDT_MODE_BOOT;
				printf("debug: Waiting for boot up\n");
			}
			break;

		case SWDT_MODE_BOOT:
			putchar('-');

			self->seconds++;
			if (self->seconds == 10)
			{
				self->mode = SWDT_MODE_WATCH;
				self->seconds = 0;
				printf("debug: Booted up. Watching..\n");
			}
			break;

		default:
			break;
	}
}

/**
 * @brief Handles a given command
 * @param cmd: ascii character, representing command
 * supported commands: '?' => prints welcome screen
 *                     '#' => clears the watchdog timer (if monitoring)
 *                     'e' => disable monitoring (if enabled)
 *                     'd' => enable monitoring (if disabled)
 */
void SmartWDT_GotCommand(struct SmartWDT *self, char cmd)
{

	switch(cmd) {
		case '?':
			puts(SmartWDT_WelcomeScreen);
			break;

		case '#':
			if (self->mode == SWDT_MODE_WATCH)
			{
				SmartWDT_ClearTimer(self);
				putchar('\n');
			}
			break;

		case 'e':
			if (self->mode == SWDT_MODE_WATCH_DISABLED)
			{
				self->mode = SWDT_MODE_WATCH;
				printf("\nWatchdog enabled\n");
			}
			break;

		case 'd':
			if (self->mode == SWDT_MODE_WATCH)
			{
				self->mode = SWDT_MODE_WATCH_DISABLED;
				SmartWDT_ClearTimer(self);
				printf("\nWatchdog disabled\n");
			}
			break;

		default:
			break;
			// TODO:What should I do?
	}
}



/**
 * @brief Initializes SmartWDT
 * Setups UART and std(in|out|err)
 */
void SmartWDT_Init(struct SmartWDT *self)
{
	// Read only
	fdev_setup_stream(&self->inout, ser_putch, ser_getch, _FDEV_SETUP_RW);
	stdout = stderr  = stdin = &self->inout;

	/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init(UART_BAUD_SELECT(9600, F_CPU), true, false);

	// Port B as an input
	DDRB = 0x00;

	// There is 'Reset' signal on port E
	UART0_DDR |= _BV(UART0_RTS_DDR);	/* set CTS pin direction - output */

	self->mode = SWDT_MODE_INITIAL;

	// Start WDT on itself
	wdt_enable(WDTO_2S);
	wdt_reset();

	// Enable interrupts
	sei();
}



struct SmartWDT app;

/**
 * @brief Enters Watch mode
 */
void SmartWDT_StartWatch(struct SmartWDT *self)
{
	//DDRD = 0x00;
	self->mode = SWDT_MODE_WATCH;

	// Read time in second from dip pins
	self->waiting_time = ~DevBoardShiftByteIn();
	printf("Waiting time: %d\n", (self->waiting_time));

	SmartWDT_ClearTimer(self);
	// Port D as an output (again)
	DDRD = 0xFF; 


	self->tmr_ticks = 0;
	// Interrupts from timer
	TIMSK = 0x04;	// Timer 1 overflow interrupt

	// Start timer
	TCCR1A = 0x00;
	TCCR1B = 0x01; // Directly from clock
	TCCR1C = 0x00;

}

/**
 * @brief Enters Setup mode
 */
void SmartWDT_StartSetup(struct SmartWDT *self)
{
	// Port D as an input
	DDRD = 0x00;

	// Disable timer
	TCCR1B = 0x00;

	self->mode= SWDT_MODE_SETUP;
}


// Timer interrupt on overflow
ISR(TIMER1_OVF_vect)
{
	app.tmr_ticks++;
	if (app.tmr_ticks == SmartWDT_TimerIntsPerSec)
	{
		app.tmr_ticks = 0;
		SmartWDT_TickTimer(&app);
	}
}


// Interrupt handler - Recieved a byte over RS232
ISR(USART0_RX_vect)
{
	SmartWDT_GotCommand(&app, UDR0);
}




/**
 * @brief Handles Watchdog reset
 * if HALT_ON_RESET is nonzero,
 * program will stop.
 * otherwise, program will start from the beginning.
 */
void WatchdogResetOccured(void)
{
	// Init std*
	FILE inout;
	fdev_setup_stream(&inout, ser_putch, NULL, _FDEV_SETUP_RW);
	stdout = stderr = &inout;

	/* open UART0 - 9600,N,8,1 - interrupt driven RxD */
	ser_init(UART_BAUD_SELECT(9600, F_CPU), false, false);

	if (HALT_ON_RESET)
	{
		printf("Watchdog reset occured. Going to infinite loop..\n");
		while(1)
			;
	} else {
		printf("Watchdog reset occured. Starting again\n");
	}
	
}

uint8_t fraction_to_graph(uint8_t numerator, uint8_t denominator)
{
	uint16_t val = numerator * 8;
	val /= denominator;
	val = 0xFF << (val);
	return (uint8_t)val;
}

int main(void)
{
	bool wd_reset = !!(MCUCSR & _BV(WDRF));
	MCUSR = 0x00;

	if (wd_reset)
	{
		WatchdogResetOccured();
	}

	SmartWDT_Init(&app);

	puts(SmartWDT_WelcomeScreen);

	reset_computer_stop();

	while(1)
	{
		wdt_reset();


		// Read Mode select
		bool mode = (PINB >> 6) & 0x01;

		if (mode == 1)
		{
			if (app.mode == SWDT_MODE_SETUP || app.mode == SWDT_MODE_INITIAL)
			{
				SmartWDT_StartWatch(&app);
				printf("Started monitoring...\n");
			}
		} else {
			if (app.mode != SWDT_MODE_SETUP)
			{
				SmartWDT_StartSetup(&app);
				printf("Setup mode...\n");
			}
		}


		switch(app.mode)
		{
			case SWDT_MODE_WATCH:
				PORTD = ~fraction_to_graph(app.seconds, app.waiting_time);
				break;

			case SWDT_MODE_WATCH_DISABLED:
				PORTD = ~fraction_to_graph(app.seconds, app.waiting_time);
				break;

			case SWDT_MODE_RESET:
				reset_computer_start();
				PORTD = app.seconds & 0x01? 0x00 : 0xFF;
				break;

			case SWDT_MODE_BOOT:
				reset_computer_stop();
				break;

			default:
				break;

		}



	}


	return 0;
}



