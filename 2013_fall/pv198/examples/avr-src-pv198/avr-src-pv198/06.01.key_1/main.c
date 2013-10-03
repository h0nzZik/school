/*
 *                  KEYBOARD - Example
 *
 * Description : Basic functions used for matrix keyboard 
 *               
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.1 2006/12/14 21:30:54 bubulak Exp $
 */ 

#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>


#define KEY_OUT		PORTD		/* port for keyboart */
#define KEY_IN		PIND		/* input pin from 74HC165 */
#define KEY_DIR		DDRD		/* pin direction bit */

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)


/* Function prototypes */
void Ser_init(uint16_t baud);
int Ser_putch(char c, FILE *stream);
int Ser_getch(FILE *stream);

uint8_t Key_scan(void);
uint8_t Key_read(void);


/*!
 * \brief M A I N   Function
 * 
 */
int main(void)
{
	uint8_t tmp;																	/* temporary */
	FILE mystdout = FDEV_SETUP_STREAM(Ser_putch, Ser_getch, _FDEV_SETUP_RW);
	stdout = stdin = stderr =  &mystdout;
	Ser_init( UART_BAUD_SELECT(9600, F_CPU) );
	while (1) {
		if (Key_scan()) {
			tmp = Key_read();														/* If we found new key, read it */
			printf ("Button number %d was pressed and released \n",tmp);
		}
	}	
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

/*!
 * \brief Function reads keyboard and return a stable key.
 *	Function waits untill button is stable. Function returns its value if button is released.
 *
 * \return key_read => Keycode of pushed button
 *
 */
uint8_t Key_read(void) {
uint8_t key_tmp = 0;
	key_tmp = Key_scan();					/* Load key value first */
    do {
        _delay_ms(50);						/* Wait a while */
		if (key_tmp == Key_scan())	        /* Do we read the same value ? */
			break;
		else 
			key_tmp = Key_scan();
    } while (1);		
	
	while (Key_scan()); 					/*  Wait till button is released */
	return (key_tmp);
}


/*!
 * \brief Function for fast scanning of keyboar
 *
 * \return key_read => It returns scan code from 1 to 16 or 0xff as error (two or more keys were pushed)
 *
 */
uint8_t Key_scan(void) {
	uint8_t key_mask;
	uint8_t key_read;
	uint8_t i;

	KEY_DIR = 0xf0;									/* Set high pins as output */
	KEY_OUT = 0xff;									/* Set pins to high */

	key_mask = 0x10;								/* Prepare a mask for reading */
	key_read = 0;
	for (i = 0; i < 4; i++) {
		KEY_OUT = ~key_mask;						/* Activate one row */
		_delay_us(10);								/* Wait a while */
		if (~KEY_IN & 0xf) {						/* Read a pins */
			switch (~KEY_IN & 0xf) {				/* Translate a value from bin to inc */
				case 1:	key_read += 1; break;
				case 2: key_read += 2; break;
				case 4: key_read += 3; break;
				case 8: key_read += 4; break;
				default:	
						key_read = 0xff; break;		/* For error set value to 0xFF */

			}
			if (key_read != 0xff)
				key_read += i * 4;					/* If not error make a correction from rows */
		}
		key_mask <<= 1;
		if (key_read)								/* If we don't have a value, try to read next row */
			break;
	}
	return (key_read);
}
