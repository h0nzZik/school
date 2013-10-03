/*
 *                  SERVO MOVING
 *
 * Description : Another type of timer settings for PWM generation with IRQ
 *               
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.1 2006/12/15 00:27:39 bubulak Exp $
 */ 

#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>



/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)


/* Function prototypes */
void Ser_init(uint16_t baud);
int Ser_putch(char c, FILE *stream);
int Ser_getch(FILE *stream);

ISR(TIMER1_COMPA_vect)
{
static uint8_t dir= 1;
static uint8_t cnt = 0;

	if (++cnt == 5) {
		cnt = 0;
		if (dir == 1) {				
			OCR1C += 10;
			PORTD ^= 1;				/* toggle LED up */
		}
		else { OCR1C -= 10;
			PORTD ^= 2;				/* toggle LED */
		}
		if (OCR1C >= 2000) {		/* When reach to max. value change a direction */
			dir = 0;
			PORTD |= 1;				/* turn off LED up */
		}
		if (OCR1C <= 1000) {		/* When reach to min. value change a direction */
			dir = 1;
			PORTD |= 2;				/* turn off LED down */
		}
	}
}


/*!
 * \brief M A I N   Function
 * 
 */
int main(void)
{
	FILE mystdout = FDEV_SETUP_STREAM(Ser_putch, Ser_getch, _FDEV_SETUP_RW);
	stdout = stdin = stderr =  &mystdout;
	Ser_init( UART_BAUD_SELECT(9600, F_CPU) );
	
	TCCR1A = _BV(COM1C1);	/* Clear OC1C on compare match */
	TCCR1B |= _BV(WGM13);	/* Mode 8 PWM, TOP is ICR1 */
	TCCR1B |= _BV(CS11);	/* Divider is 8 */
	ICR1 = 18432;			/* Time of frame 20ms */
	OCR1C = 1000;			/* High pulse 1ms */	
	DDRB |= 0xc0;

	DDRD = 0x3;				/* set port D as output */
	PORTD = 0x3;			/* turn off all port D LEDs */



	printf ("\nPress a button to start servo moving. \n");
	getchar();
	TIMSK  = _BV(OCIE1A);	/* enable timer 1 output compare A match interrupt */
	sei();					/* global interrupt enable */
	while (1);
	return(0);
}



/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baudrate generator value.
 */
void Ser_init(uint16_t baud)
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
int Ser_putch(char c, FILE *stream)
{
	if (c == '\n')
		Ser_putch('\r', stream);
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
int Ser_getch(FILE *stream)
{
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* get and return received data from buffer */
	return UDR0;
}
