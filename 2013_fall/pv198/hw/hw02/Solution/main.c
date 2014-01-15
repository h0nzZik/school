/**
 * @file main.c
 * @brief Serial <--> LCD czech demo.
 * Description : Extended functions used for 4-bit communication with LCD
 *               between ATmega128 and 4 lines LCD MC4004
 *
 * @author Zbynek Bures, Jan Tusil
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.2 2006/12/03 11:03:26 cvsd Exp $
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "shift.h"

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);
/* Function prototypes */
void Hw_init(void);


/*!
 *      M       A       I       N
 *
 */
int main(void)
{
		uint8_t i;

		static const char *s = "Zvláš zákeønı uèeò s ïolíèky bìí podél zóny úlù.";

		FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);
		stdout = stdin = stderr =  &mystdout;
		ser_init( UART_BAUD_SELECT(9600, F_CPU) );


		printf("Debug: program started\r\n");


        Hw_init();
		


        LCD_cursor_yx(1,1);                     /* Display first line */



		uint8_t j = 0;
		while (j < 60 && s[j])
		{
			LCD_putc(s[j++]);
		}


		while(1)
		{
			LCD_putc(ser_getch(NULL));
		}

        return(0);
}

/*!
 * \brief Function Hw_init sets basic HW configuration
 */
void Hw_init(void)
{      
	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR);	/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);	/* serial data stream -> output pin */ 
	SHIFT_DDR |= _BV(SHIFT_LCD_SET_DDR);	/* parallel write -> output pin */

    LCD_init();                             /* Init LCD display */
	czech_init();
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
